#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <expected>

enum class ListenerErr {
    AcceptErr,
};

class Socket
{
protected:
    int family;
    int type;
    int fd;

public:
    Socket(int family, int type, int fd);
    ~Socket();
    int getFd() const { return fd; }
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;
    Socket(Socket &&other) noexcept : family(other.family), type(other.type), fd(other.fd)
    {
        other.fd = -1;
    }
};

class Listener : public Socket
{

public:
    Listener(int family, int type);
    void bindAddress(const sockaddr_in &address);

    void listenConnections(int connections);
    pollfd getPollConfig() const;

    std::expected<int, ListenerErr> acceptConnection(sockaddr_in &client_address);
private:
    void enableAddressReuse();
};

#endif