#include "connection_manager.h"

#include "http_request.h"

ConnectionManager::ConnectionManager(int family, int connection_type)
    : family(family),
      connection_type(connection_type),
      listener(std::make_unique<ListeningSocket>(family, connection_type))

{
    listener->bindAddress(generateLocalAddress());
    listener->listenConnections(Connections);
    updatePollFds();
}

void ConnectionManager::updatePollFds()
{
    poll_fds.clear();

    poll_fds.push_back({listener->getFd(), POLLIN, 0});

    for (auto& sock : client_sockets)
    {
        poll_fds.push_back({sock->getFd(), POLLIN, 0});
    }
}

sockaddr_in ConnectionManager::generateLocalAddress() const
{
    struct sockaddr_in address{};
    memset(&address, 0, sizeof(address));

    address.sin_family = family;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(ListenerPort);

    return address;
}
void ConnectionManager::acceptConnection()
{
    struct sockaddr_in client_addr{};
    std::unique_ptr<Socket> data_socket = std::make_unique<Socket>(
        family, connection_type, listener->acceptConnection(client_addr));

    poll_fds.push_back({data_socket->getFd(), POLLIN, 0});

    client_sockets.push_back(std::move(data_socket));

    logConnection(client_addr);
}
void ConnectionManager::logConnection(
    const struct sockaddr_in& client_addr) const
{
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr.sin_port);
    std::cout << "New connection from " << client_ip << ":" << client_port
              << std::endl;
}
void ConnectionManager::removeDisconnectedClients(
    std::vector<size_t>& disconnected_clients_socket_idxs)
{
    if (!disconnected_clients_socket_idxs.empty())
    {
        for (size_t i : disconnected_clients_socket_idxs)
        {
            client_sockets.erase(client_sockets.begin() + i);
        }
        updatePollFds();
    }
}
void ConnectionManager::pollForEvents(const PathForwarder& path_forwarder)
{
    if (connectionsPending())
    {
        std::vector<size_t> disconnected_clients_socket_idxs;
        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            if (poll_fds[i].revents == 0)
                continue;
            auto revents = poll_fds[i].revents;
            int fd = -1;
            if (i > 0)
                fd = client_sockets[i - 1]->getFd();

            if (revents & POLLHUP || revents & POLLNVAL || revents & POLLERR)
            {
                std::cerr << "Client disconnected (fd: " << fd << ")\n";
                disconnected_clients_socket_idxs.push_back(i - 1);
                continue;
            }

            else if (poll_fds[i].fd == listener->getFd() &&
                     (poll_fds[i].revents & POLLIN))
            {
                acceptConnection();
                poll_fds[i].revents = 0;
            }
            else if (poll_fds[i].revents & POLLIN)
            {
                try
                {
                    request_handler.handleRequest(
                        client_sockets[i - 1]->getFd(), path_forwarder);
                    poll_fds[i].revents = 0;
                }
                catch (std::exception& e)
                {
                    disconnected_clients_socket_idxs.push_back(i - 1);
                }
            }
        }
        removeDisconnectedClients(disconnected_clients_socket_idxs);
    }
}
bool ConnectionManager::connectionsPending() const
{
    int pending =
        poll(const_cast<pollfd*>(poll_fds.data()), poll_fds.size(), 100);
    if (pending < 0)
        throw std::system_error(errno, std::system_category(),
                                "Error during polling");

    return pending > 0;
}