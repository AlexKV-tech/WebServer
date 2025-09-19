#include "server.h"
#include "request.h"

#include <format>
#include <iostream>
#include <stdexcept>
#include <utility>

Server::Server(int family, int connection_type)
    : family(family)
    , connection_type(connection_type)
    , listener(std::make_unique<ListeningSocket>(family, connection_type))
    , path_forwarder()
{
    listener->bindAddress(generateLocalAddress());
    listener->listenConnections(Connections);
    updatePollFds();
}

void Server::updatePollFds()
{
    poll_fds.clear();

    poll_fds.push_back({ listener->getFd(), POLLIN, 0 });

    for (auto& sock : client_sockets) {
        poll_fds.push_back({ sock->getFd(), POLLIN, 0 });
    }
}

sockaddr_in Server::generateLocalAddress() const
{
    struct sockaddr_in address {};
    memset(&address, 0, sizeof(address));

    address.sin_family = family;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(ListenerPort);

    return address;
}

void Server::run()
{
    while (true) {
        try {
            if (connectionsPending()) {
                std::vector<size_t> invalid_socket_indexes;
                handleEvents(invalid_socket_indexes);
                if (!invalid_socket_indexes.empty()) {
                    for (size_t i : invalid_socket_indexes) {
                        client_sockets.erase(client_sockets.begin() + i);
                    }
                    updatePollFds();
                }
            }
        } catch (std::exception& err) {
            std::cerr << err.what();
        }
    }
}

void Server::handleEvents(std::vector<size_t>& invalid_socket_indexes)
{
    for (size_t i = 0; i < poll_fds.size(); i++) {
        if (poll_fds[i].revents == 0)
            continue;

        auto revents = poll_fds[i].revents;
        int fd = -1;
        if (i > 0)
            fd = client_sockets[i - 1]->getFd();

        if (revents & POLLHUP || revents & POLLNVAL || revents & POLLERR) {
            std::cerr << "Client disconnected (fd: " << fd << ")\n";
            invalid_socket_indexes.push_back(i - 1);
            continue;
        }

        else if (poll_fds[i].fd == listener->getFd() && (poll_fds[i].revents & POLLIN)) {
            acceptConnection();
            poll_fds[i].revents = 0;
        } else if (poll_fds[i].revents & POLLIN) {
            auto data = receiveFromClient(i - 1);
            std::cout << "{"
                      << data << "}";
            sendResponseToClient(HTTPRequest::parseMethodRequestPath(data, "GET"), i - 1);
            std::cout << "POST: " << HTTPRequest::parseMethodRequestPath(data, "POST") << '\n';
            poll_fds[i].revents = 0;
        }
    }
}

bool Server::connectionsPending() const
{
    int pending = poll(const_cast<pollfd*>(poll_fds.data()), poll_fds.size(), 100);
    if (pending < 0)
        throw std::system_error(errno, std::system_category(),
            "Error during polling");

    return pending > 0;
}

void Server::acceptConnection()
{
    struct sockaddr_in client_addr {};
    std::unique_ptr<Socket> data_socket = std::make_unique<Socket>(
        family, connection_type, listener->acceptConnection(client_addr));

    poll_fds.push_back({ data_socket->getFd(), POLLIN, 0 });

    client_sockets.push_back(std::move(data_socket));

    logConnection(client_addr);
}

void Server::logConnection(const struct sockaddr_in& client_addr) const
{
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr.sin_port);
    std::cout << "New connection from " << client_ip << ":" << client_port
              << std::endl;
}

bool Server::sendResponseToClient(const std::filesystem::path& filename,
    size_t client_num) const
{
    if (client_num >= client_sockets.size())
        throw std::length_error("There is no client with such a number " + std::to_string(client_num));

    std::string response = path_forwarder.generateHttpResponse(filename);

    return send(client_sockets[client_num]->getFd(), response.c_str(),
               response.size(), 0)
        >= 0;
}

std::string Server::receiveFromClient(size_t client_num)
{

    if (client_num >= client_sockets.size())
        throw std::length_error("There is no client with such a number " + std::to_string(client_num));

    int client_fd = client_sockets[client_num]->getFd();
    std::string request;
    char buffer[BufferSize];
    auto [bytes_read, header_end] = HTTPRequest::readHeader(client_fd, buffer, request);

    // Check for errors/premature disconnection after header reading
    if (bytes_read < 0) {
        std::cerr << "Connection closed (fd: " << client_fd << ")" << std::endl;
        client_sockets.erase(client_sockets.begin() + client_num);
        throw std::system_error(
            errno, std::system_category(),
            std::format("Failed to fetch data from client {}", client_num));
    } else if (bytes_read == 0 && request.empty()) {
        std::cerr << "Connection closed by client (fd: " << client_fd << ")" << std::endl;
        client_sockets.erase(client_sockets.begin() + client_num);
        throw std::runtime_error("Client closed connection before sending any data");
    }

    if (header_end != std::string::npos)
        HTTPRequest::readBody(client_fd, buffer, request, header_end);

    return request;
}

void Server::setPathMapping(const std::filesystem::path& requested_path,
    const std::filesystem::path& response_path)
{
    path_forwarder.addForwardingRule(requested_path, response_path);
}

void Server::setPathMapping(
    const std::map<std::filesystem::path, std::filesystem::path>& routes)
{
    path_forwarder.addForwardingRules(routes);
}
