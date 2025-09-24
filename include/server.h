#ifndef SERVER_H
#define SERVER_H

#include "path_forward.h"
#include "request.h"
#include "socket.h"

#include <arpa/inet.h>
#include <ctime>
#include <map>
#include <memory>
#include <poll.h>
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

    PathForwarder path_forwarder;

public:
    Server(int family, int connection_type);
    /**
     * @brief Starts the server
     */
    void run();
    /**
     * @brief Maps a single HTTP request path to a filesystem location
     *
     * Configures the server to serve a specific file when the given HTTP path
     * is requested.
     *
     * @param requested_path HTTP request path (e.g., "/", "/about")
     * @param response_path Filesystem path to the file to serve (e.g.,
     * "./static/index.html")
     *
     * @example
     * @code
     * server.setPathMapping("/", "/computer/project/static/index.html");
     * server.setPathMapping("/favicon.ico", "./assets/favicon.ico");
     * @endcode
     */
    void setPathMapping(const std::string& method, const std::filesystem::path& requested_path,
        const std::filesystem::path& response_path);
    /**
     * @brief Maps HTTP request paths to filesystem locations
     *
     * Configures the server to serve files by mapping HTTP request URLs
     * to specific filesystem paths.
     *
     * @param routes Map where:
     *               - Key: HTTP request path (e.g., "/", "/about")
     *               - Value: Filesystem path (e.g., "./static/index.html")
     *
     * @example
     * @code
     * std::map<std::filesystem::path, std::filesystem::path> routes = {
     *     {"/", "/computer/project/static/index.html"},
     *     {"/about", "/computer/project/static/about.html"}
     * };
     * server.setFileRoutes(routes);
     * @endcode
     */
    void setPathMapping(
        const std::map<std::pair<std::string, std::string>, std::filesystem::path>& routes);

private:
    void updatePollFds();
    sockaddr_in generateLocalAddress() const;
    void handleEvents(std::vector<size_t>& invalid_socket_indexes);
    bool connectionsPending() const;
    void acceptConnection();
    void logConnection(const struct sockaddr_in& client_addr) const;
    bool sendResponseToClient(const std::string& method, const std::filesystem::path& filename,
        size_t client_num) const;
    HTTPRequest receiveFromClient(size_t client_num);
};

#endif