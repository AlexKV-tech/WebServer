#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <expected>
#include <memory>
#include <vector>

#include "path_forwarder.hpp"
#include "socket.hpp"

enum class ConnectionManagerErr {
    PollErr,
    acceptErr,
};

class ConnectionManager {
    int family;
    int connection_type;

    std::unique_ptr<Listener> listener;
    std::vector<std::unique_ptr<Socket>> client_sockets;
    std::vector<pollfd> client_poll_fds;
    static constexpr int ListenerPort = 7777;
    static constexpr int Connections = 100;

  public:
    ConnectionManager(int family, int connection_type);
    std::expected<void, ConnectionManagerErr>
    pollForEvents(const PathForwarder &path_forwarder);

  private:
    [[nodiscard]] sockaddr_in generateLocalAddress() const;
    std::expected<void, ConnectionManagerErr> acceptConnection();
    static void logConnection(const sockaddr_in &client_addr);
};
#endif