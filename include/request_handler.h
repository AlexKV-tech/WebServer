#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <filesystem>

#include "http_method.h"
#include "http_request.h"
#include "path_forwarder.h"

enum class RequestHandlerErr {
    SendErr,
};

class RequestHandler
{
public:
    static std::expected<void, RequestHandlerErr> handleRequest(int client_fd, const PathForwarder& path_forwarder);
private:
    static HttpRequest receiveRequestFromClient(int client_fd);
    [[nodiscard]] static std::expected<void, RequestHandlerErr> sendResponseToClient(HttpMethod method,
                                                   const std::filesystem::path& filename,
                                                   int client_fd,
                                                   const PathForwarder& path_forwarder);
};

#endif