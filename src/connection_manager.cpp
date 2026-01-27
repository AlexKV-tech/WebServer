#include "connection_manager.h"

#include "http_request.h"

#include <algorithm>

ConnectionManager::ConnectionManager(int family, int connection_type)
    : family(family),
      connection_type(connection_type),
      listener(std::make_unique<Listener>(family, connection_type))
{
    listener->bindAddress(generateLocalAddress());
    listener->listenConnections(Connections);
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

void ConnectionManager::logConnection(
    const struct sockaddr_in &client_addr) const
{
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr.sin_port);
    std::cout << "New connection from " << client_ip << ":" << client_port
              << std::endl;
}

void ConnectionManager::pollForEvents(const PathForwarder &path_forwarder)
{

    std::vector<pollfd> fds;
    fds.reserve(client_sockets.size() + 1);

    fds.push_back(listener->getPollConfig());

    for (const auto &sock : client_sockets)
    {
        fds.push_back({sock->getFd(), POLLIN, 0});
    }

    int pending = poll(fds.data(), fds.size(), 100);

    if (pending < 0)
    {
        throw std::runtime_error("Poll error");
    }

    if (pending == 0)
    {
        std::cout << "No connections pending" << std::endl;
        return;
    }

    std::vector<size_t> to_remove;

    if (fds[0].revents & POLLIN)
    {
        acceptConnection();
    }

    for (size_t i = 1; i < fds.size(); ++i)
    {
        if (fds[i].revents == 0)
            continue;

        size_t client_idx = i - 1;

        if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
        {
            to_remove.push_back(client_idx);
        }
        else if (fds[i].revents & POLLIN)
        {
            try
            {
                request_handler.handleRequest(client_sockets[client_idx]->getFd(), path_forwarder);
            }
            catch (...)
            {
                to_remove.push_back(client_idx);
            }
        }
    }

    if (!to_remove.empty())
    {
        std::sort(to_remove.rbegin(), to_remove.rend());
        for (size_t idx : to_remove)
        {
            client_sockets.erase(client_sockets.begin() + idx);
        }
    }
}

void ConnectionManager::acceptConnection()
{
    struct sockaddr_in client_addr{};

    client_sockets.push_back(std::make_unique<Socket>(
        family, connection_type, listener->acceptConnection(client_addr)));

    logConnection(client_addr);
}