#include "request_handler.h"

#include "request_generator.h"

HttpRequest RequestHandler::receiveRequestFromClient(int client_fd)
{
    auto http_req = HttpRequest(client_fd);

    return http_req;
}
bool RequestHandler::sendResponseToClient(
    HttpMethod method, const std::filesystem::path &filename, int client_fd,
    const PathForwarder &path_forwarder) const
{
    std::string response = RequestGenerator::generateHttpResponse(
        method, filename, path_forwarder);

    return send(client_fd, response.c_str(), response.size(), 0) >= 0;
}
bool RequestHandler::handleRequest(int client_fd,
                                   const PathForwarder &path_forwarder)
{
    HttpRequest data = receiveRequestFromClient(client_fd);
    std::cout << "{" << data.getBody() << "}";

    bool success = sendResponseToClient(data.getMethod(), data.getPath(),
                                        client_fd, path_forwarder);
    std::cout << "POST: " << data.getPath() << '\n';
    return success;
}