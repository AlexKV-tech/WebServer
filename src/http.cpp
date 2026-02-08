#include <expected>
#include <fstream>
#include <iostream>
#include <string_view>
#include <sys/socket.h>

#include "http.hpp"
#include "http_types.hpp"

namespace Http {

std::expected<Request, ParseErr> parse(int fd, Request &http_request) {
    std::string request_headers{""};
    char buffer[Request::BUF_SIZE];

    ssize_t bytes_read;
    size_t request_headers_end = std::string::npos;

    // Phase 1: Read until get request line and all headers (according to
    // HTTP until \r\n\r\n)
    while ((bytes_read = recv(fd, buffer, sizeof(buffer), 0)) > 0) {
        request_headers.append(buffer, bytes_read);

        if ((request_headers_end = request_headers.find(Request::HEADERS_END)) !=
            std::string::npos)
            break;
    }
    

    
    if (bytes_read < 0) {
        std::cerr << "Connection closed (fd: " << fd << ")\n";
        return std::unexpected(ParseErr::FetchErr);
    } else if (bytes_read == 0 && request_headers.empty()) {
        std::cerr << "Connection closed by client (fd: " << fd << ")\n";
        return std::unexpected(ParseErr::PrematureDisconnectionErr);
    }

    
    size_t request_line_end = request_headers.find(Request::HEADERS_SPACE);
    if (request_line_end == std::string_view::npos) {
        std::cerr << "Invalid HTTP request provided. Note: request lines must be separated by \\r\\n \n";
        return std::unexpected(ParseErr::InvalidHttpErr);
    }
        
    std::string_view request_headers_sv{request_headers};
    std::string_view request_line{request_headers_sv.substr(0, request_line_end)};

    size_t first_space = request_line.find(' ');
    if (first_space == std::string_view::npos) {
        std::cerr << "Invalid HTTP request provided. Note: No space after method\n";
        return std::unexpected(ParseErr::InvalidHttpErr);
    }
        

    size_t second_space = request_line.find(' ', first_space + 1);
    if (second_space == std::string_view::npos) {
        std::cerr << "Invalid HTTP request provided. Note: No space after URL\n";
        return std::unexpected(ParseErr::InvalidHttpErr);
    }
        

    std::string_view method_sv{request_line.substr(0, first_space)};

    std::string_view url_sv{
        request_line.substr(first_space + 1, second_space - (first_space + 1))};

    std::string_view ver_sv{request_line.substr(second_space + 1)};

    http_request.method = str_to_method(method_sv);

    http_request.url = std::string(url_sv);
    http_request.version = std::string(ver_sv);

    size_t start = 0;
    size_t end;

    // Skip first line
    start = request_headers_sv.find(Request::HEADERS_SPACE) +
          Request::HEADERS_SPACE.size();
    std::string_view request_header_sv;
    // Read all headers beginning from second line
    while ((end = request_headers_sv.find(Request::HEADERS_SPACE, start)) !=
           std::string_view::npos) {
        request_header_sv = request_headers_sv.substr(start, end - start);
        if (request_header_sv.empty()) {
            break; // encountered \r\n\r\n
        }
             
        if (auto colon = request_header_sv.find(':'); colon != std::string_view::npos) {
            std::string_view key{request_header_sv.substr(0, colon)};
            std::string_view value{request_header_sv.substr(colon + 1)};

            if (auto first_not_space = value.find_first_not_of(' ');
                first_not_space != std::string_view::npos) {
                value.remove_prefix(first_not_space);
            }

            if (auto last_not_space = value.find_last_not_of(' ');
                last_not_space != std::string_view::npos) {
                value.remove_suffix(value.size() - (last_not_space + 1));
            }

            http_request.headers.insert({std::string(key), std::string(value)});
        }
        start = end + Request::HEADERS_SPACE.size();
    }

    size_t content_length{0};
    if (http_request.headers.contains("Content-Length")) {
        content_length =
            std::stoul(http_request.headers.at("Content-Length"));
    }
    else { // if no Content-Length header is present, then the body is empty
        return http_request;
    }
        

    size_t body_start_pos = request_headers_end + Request::HEADERS_END.size();

    if (body_start_pos < request_headers.size()) { // check whether a body was partially read during headers reading
        http_request.body = request_headers.substr(body_start_pos);
        
    }

    size_t body_received = http_request.body.size();

    while (body_received < content_length) {
        ssize_t bytes_read =
            recv(fd, buffer,
                 std::min(static_cast<size_t>(Request::BUF_SIZE),
                          content_length - body_received),
                 0);

        if (bytes_read < 0) {
            std::cerr << "Error during body read\n";
            return std::unexpected(ParseErr::BodyReadErr);
        }

        if (bytes_read == 0) {
            break;
        }
            

        http_request.body.append(buffer, bytes_read);
        body_received += bytes_read;
    }
    return http_request;
}
std::string generate_response(Method method,
                              const std::filesystem::path &requested_path,
                              const PathForwarder &path_forwarder) {

    if (auto path = path_forwarder.find_server_path(
            std::make_pair(method_to_str(method), requested_path.native()));
        path.has_value()) {
        const std::filesystem::path &server_path = path.value();
        auto content_type =
            path_forwarder.get_mime_type(server_path.extension().native());
        std::ifstream fs{server_path, std::ios::in | std::ios::binary};

        std::string html_content{std::istreambuf_iterator<char>(fs),
                                 std::istreambuf_iterator<char>()};

        fs.close();
        return std::format("HTTP/1.1 200 OK\r\n"
                           "Content-Type: {}\r\n"
                           "Content-Length: {}\r\n"
                           "\r\n"
                           "{}",
                           content_type, html_content.size(), html_content);
    }

    std::string err_response =
        std::format("<html>"
                    "<head><title> 404 Not Found</title></head>"
                    "<body>"
                    "<h1>Not Found</ h1>"
                    "<p>The requested URL/ {} was not found on this server.</p>"
                    "</body>"
                    "</html>",
                    requested_path.string());
    return std::format("HTTP/1.1 404 Not Found\r\n"
                       "Content-Type: {}\r\n"
                       "Content-Length: {}\r\n"
                       "\r\n"
                       "{}",
                       "text/html", err_response.size(), err_response);
}

} // namespace Http