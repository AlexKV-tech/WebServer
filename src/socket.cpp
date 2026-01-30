#include <system_error>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.h"

Socket::Socket(int family, int type, int fd)
    : family(family), type(type), fd(fd)
{

    if (fd < 0) {
        throw std::system_error(errno, std::system_category(), "Socket creation failed");
    }
}

Socket::~Socket()
{
    if (fd >= 0)
    {
        close(fd);
    }
}

Listener::Listener(int family, int type)
    : Socket(family, type, socket(family, type, 0))
{
}

void Listener::bindAddress(const sockaddr_in &address)
{
    enableAddressReuse();
    if (bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) < 0)
        throw std::system_error(errno, std::system_category(), "Failed to bind");
}

void Listener::enableAddressReuse()
{
    int state = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state));
}

void Listener::listenConnections(int connections)
{
    if (listen(fd, connections) < 0)
        throw std::system_error(errno, std::system_category(), "Listen failed");
}

pollfd Listener::getPollConfig() const
{
    return {fd, POLLIN, 0};
}

std::expected<int, ListenerErr> Listener::acceptConnection(sockaddr_in &client_address)
{
    socklen_t client_address_size = sizeof(client_address);
    int client_fd = accept(this->fd, reinterpret_cast<sockaddr *>(&client_address), &client_address_size);

    if (client_fd < 0)
        return std::unexpected(ListenerErr::AcceptErr);

    return client_fd;
}