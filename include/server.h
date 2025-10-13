#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <poll.h>

#include <ctime>
#include <map>
#include <memory>
#include <vector>

#include "connection_manager.h"
#include "http_method.h"
#include "http_request.h"
#include "path_forwarder.h"
#include "socket.h"

class Server
{
private:
    ConnectionManager connection_manager;
    PathForwarder path_forwarder;

public:
    Server(int family, int connection_type);
    /**
     * @brief Starts the server
     */
    void run();
    /**
     * @brief Maps a single Http request path to a filesystem location
     *
     * Configures the server to serve a specific file when the given Http path
     * is requested.
     *
     * @param requested_path Http request path (e.g., "/", "/about")
     * @param response_path Filesystem path to the file to serve (e.g.,
     * "./static/index.html")
     *
     * @example
     * @code
     * server.setPathMapping("/", "/computer/project/static/index.html");
     * server.setPathMapping("/favicon.ico", "./assets/favicon.ico");
     * @endcode
     */
    void setPathMapping(HttpMethod method,
                        const std::filesystem::path& requested_path,
                        const std::filesystem::path& response_path);
    /**
     * @brief Maps Http request paths to filesystem locations
     *
     * Configures the server to serve files by mapping Http request URLs
     * to specific filesystem paths.
     *
     * @param routes Map where:
     *               - Key: Http request path (e.g., "/", "/about")
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
    void setPathMapping(const std::map<std::pair<std::string, std::string>,
                                       std::filesystem::path>& routes);
};

#endif