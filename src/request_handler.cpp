#include <iostream>
#include <sys/socket.h>

#include "request_handler.h"
#include "request_generator.h"

HttpRequest RequestHandler::receiveRequestFromClient(int client_fd)
{
    auto http_req = HttpRequest(client_fd);

    return http_req;
}
std::expected<void, RequestHandlerErr> RequestHandler::sendResponseToClient(
    HttpMethod method, const std::filesystem::path &filename, int client_fd,
    const PathForwarder &path_forwarder) {
    std::string response = RequestGenerator::generateHttpResponse(
        method, filename, path_forwarder);
    if (send(client_fd, response.c_str(), response.size(), MSG_NOSIGNAL) >= 0)
        return {};
    return std::unexpected(RequestHandlerErr::SendErr);
}
std::expected<void, RequestHandlerErr> RequestHandler::handleRequest(int client_fd,
                                   const PathForwarder &path_forwarder)
{
    HttpRequest data = receiveRequestFromClient(client_fd);
    std::cout << "{" << data.getBody() << "}";

    auto send_res = sendResponseToClient(data.getMethod(), data.getPath(),
                                        client_fd, path_forwarder);
    std::cout << "POST: " << data.getPath() << '\n';
    return send_res;
}