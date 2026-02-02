#include <iostream>
#include <sys/socket.h>

#include "http.hpp"
#include "request_handler.hpp"

namespace {

std::expected<void, RequestHandlerErr>
send_response(Http::Method method, const std::filesystem::path &filename,
              int client_fd, const PathForwarder &path_forwarder) {
    std::string response =
        Http::generate_response(method, filename, path_forwarder);
    if (send(client_fd, response.c_str(), response.size(), MSG_NOSIGNAL) >= 0)
        return {};
    return std::unexpected(RequestHandlerErr::SendErr);
}
} // namespace

namespace RequestHandler {
std::expected<void, RequestHandlerErr>
handle(int client_fd, const PathForwarder &path_forwarder) {
    auto req = Http::Request{};
    auto http_req = Http::parse(client_fd, req);
    if (!http_req)
        return std::unexpected(RequestHandlerErr::SendErr);
    Http::Request data = http_req.value();
    std::cout << "{" << data.body << "}";

    auto send_res =
        send_response(data.method, data.url, client_fd, path_forwarder);
    std::cout << "POST: " << data.url << '\n';
    return send_res;
}
} // namespace RequestHandler
