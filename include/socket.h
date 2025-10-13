#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>

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
};

class ListeningSocket : public Socket
{
public:
    ListeningSocket(int family, int type);
    void bindAddress(const struct sockaddr_in& address);
    void enableAddressReuse();
    void listenConnections(int connections);
    int acceptConnection(sockaddr_in& client_address);
};

#endif