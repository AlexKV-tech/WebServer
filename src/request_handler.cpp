#include <iostream>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>

#include "http.hpp"
#include "request_handler.hpp"

namespace {

std::expected<void, RequestHandlerErr>
send_response(Http::Method method, const std::filesystem::path &filename,
              int client_fd, const PathForwarder &path_forwarder) {
    size_t total_sent = 0;
    ssize_t current_sent = 0;
    std::string response =
        Http::generate_response(method, filename, path_forwarder);
    while (total_sent < response.size()) {
        if ((current_sent = send(client_fd, response.c_str(), response.size(),
                                 MSG_NOSIGNAL)) <= 0)
            return std::unexpected(RequestHandlerErr::SendErr);
        total_sent += current_sent;
    }
    return {};
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
    std::cout << "Request: " << data.url << '\n';
    return send_res;
}
} // namespace RequestHandler
