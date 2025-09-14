#include "socket.h"
#include <filesystem>

Socket::Socket(int family, int type, int fd)
    : family(family)
    , type(type)
    , fd(fd)
{
    assert((family == AF_INET) && type == SOCK_STREAM); // Implementantion is IPv4 and TCP only
    if (fd < 0)
        throw std::system_error(errno, std::system_category(), "Failed to create socket");
}

Socket::~Socket()
{
    if (fd >= 0) {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}

ListeningSocket::ListeningSocket(int family, int type)
    : Socket(family, type, socket(family, type, 0))
{
}

void ListeningSocket::bindAddress(const struct sockaddr_in& address)
{
    enableAddressReuse();

    if (bind(fd, (const struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::system_error(errno, std::system_category(), "Failed to bind socket");
}
void ListeningSocket::enableAddressReuse()
{
    int state = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state)) < 0)
        throw std::system_error(errno, std::system_category(), "Failed to allow local address reuse");
}

void ListeningSocket::listenConnections(int connections)
{
    if (listen(fd, connections) < 0)
        throw std::system_error(errno, std::system_category(), "Failed to create listening socket");
}
int ListeningSocket::acceptConnection(sockaddr_in& client_address)
{
    int client_fd = -1;
    unsigned long client_address_size = sizeof(client_address);
    if ((client_fd = accept(this->fd, (sockaddr*)&client_address,
             (socklen_t*)&client_address_size))
        < 0)
        throw std::system_error(errno, std::system_category(), "Failed to create data transfer socket");

    return client_fd;
}