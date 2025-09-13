#ifndef SERVER_H
#define SERVER_H

#include "path_forward.h"
#include "socket.h"

#include <arpa/inet.h>
#include <poll.h>

#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>

class Server {
private:
    int family;
    int connection_type;

    std::unique_ptr<ListeningSocket> listener;
    std::vector<std::unique_ptr<Socket>> client_sockets;
    std::vector<pollfd> poll_fds;
    static constexpr int ListenerPort = 7777;
    static constexpr int Connections = 100;
    static constexpr size_t BufferSize = 8192;

    PathForwarder path_forwarder;

public:
    Server(int family, int connection_type);
    void run();
    void setStaticFilesForwarding(const std::string& requested_path,
        const std::string& response_path);
    void setStaticFilesForwarding(const std::map<std::string, std::string>& routes);

private:
    void updatePollFds();
    sockaddr_in generateLocalAddress() const;
    void handleEvents();
    bool connectionsPending() const;
    void acceptConnection();
    void logConnection(const struct sockaddr_in& client_addr) const;
    bool sendResponseToClient(const std::string& filename, size_t client_num) const;
    std::string receiveFromClient(size_t client_num);
    static std::string parseRequestPath(const std::string& http_request);
};

#endif