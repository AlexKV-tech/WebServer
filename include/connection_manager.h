#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#include <arpa/inet.h>

#include <iostream>
#include <memory>
#include <vector>

#include "request_handler.h"
#include "socket.h"
class ConnectionManager
{
private:
    int family;
    int connection_type;

    std::unique_ptr<Listener> listener;
    std::vector<std::unique_ptr<Socket>> client_sockets;
    std::vector<pollfd> client_poll_fds;
    static constexpr int ListenerPort = 7777;
    static constexpr int Connections = 100;

    RequestHandler request_handler;

public:
    ConnectionManager(int family, int connection_type);
    void updatePollFds();
    sockaddr_in generateLocalAddress() const;
    void acceptConnection();
    void logConnection(const struct sockaddr_in &client_addr) const;
    void removeDisconnectedClients(
        std::vector<size_t> &disconnected_clients_socket_idxs);
    void pollForEvents(const PathForwarder &path_forwarder);
    bool connectionsPending() const;
};
#endif