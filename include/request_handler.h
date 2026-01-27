#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <filesystem>
#include <iostream>

#include "http_method.h"
#include "http_request.h"
#include "path_forwarder.h"

class RequestHandler
{
public:
    HttpRequest receiveRequestFromClient(int client_fd);
    bool sendResponseToClient(HttpMethod method,
                              const std::filesystem::path& filename,
                              int client_fd,
                              const PathForwarder& path_forwarder) const;
    bool handleRequest(int client_fd, const PathForwarder& path_forwarder);
};

#endif