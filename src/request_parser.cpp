#include <sys/socket.h>

#include "request_parser.h"

RequestParser::RequestParser(int client_fd, HttpRequest &http_request)
{
    std::string request_head;
    char buffer[BufferSize];

    auto [bytes_read, request_head_end] =
        readRequestHead(client_fd, buffer, request_head).value();

    initRequestLine(request_head, http_request);
    initHeaders(request_head, http_request);

    if (headers.contains("Content-Length"))
        initBody(client_fd, buffer, request_head, body, request_head_end);
}
std::expected<std::pair<ssize_t, size_t>, RequestParserErr> RequestParser::readRequestHead(
    int client_fd, char *buffer, std::string &request_head)
{
    ssize_t bytes_read;
    size_t request_head_end = std::string::npos;

    // Phase 1: Read until we get request line and all headers (until
    // \r\n\r\n)
    while ((bytes_read = recv(client_fd, buffer, sizeof(buffer), 0)) > 0)
    {
        request_head.append(buffer, bytes_read);

        if ((request_head_end = request_head.find(HeadersEnd)) !=
            std::string::npos)
            break;
    }

    // Check for errors/premature disconnection after header reading
    if (bytes_read < 0)
    {
        std::cerr << "Connection closed (fd: " << client_fd << ")"
                  << std::endl;
        return std::unexpected(RequestParserErr::FetchErr);
    }
    else if (bytes_read == 0 && request_head.empty())
    {
        std::cerr << "Connection closed by client (fd: " << client_fd << ")"
                  << std::endl;
        return std::unexpected(RequestParserErr::PrematureDisconnectionErr);
    }
    return std::make_pair(bytes_read, request_head_end);
}

std::expected<void, RequestParserErr> RequestParser::initRequestLine(std::string_view request_head,
                                    HttpRequest &http_request)
{

    size_t line_end = request_head.find('\n');


    if (line_end == std::string_view::npos)
        return std::unexpected(RequestParserErr::InvalidHttpErr);


    std::string_view line = request_head.substr(0, line_end);


    if (line.ends_with('\r')) {
        line.remove_suffix(1);
    }

    size_t first_space = line.find(' ');
    if (first_space == std::string_view::npos)
        return std::unexpected(RequestParserErr::InvalidHttpErr);


    size_t second_space = line.find(' ', first_space + 1);
    if (second_space == std::string_view::npos)
        return std::unexpected(RequestParserErr::InvalidHttpErr);


    std::string_view method_sv = line.substr(0, first_space);

    std::string_view url_sv = line.substr(first_space + 1, second_space - (first_space + 1));

    std::string_view ver_sv = line.substr(second_space + 1);


    http_request.setMethod(stringToHttpMethod(method_sv));


    http_request.setUrl(url_sv);
    http_request.setVersion(ver_sv);

    return {};
}

void RequestParser::initHeaders(std::string_view request_head, HttpRequest &http_request)
{
    size_t pos = 0;
    size_t end;

    // Skip first line
    pos = request_head.find("\r\n") + 2;

    while ((end = request_head.find("\r\n", pos)) != std::string_view::npos)
    {
        std::string_view line = request_head.substr(pos, end - pos);
        if (line.empty())
            break; // found \r\n\r\n


        if (auto colon = line.find(':'); colon != std::string_view::npos)
        {
            std::string_view key = line.substr(0, colon);
            std::string_view value = line.substr(colon + 1);

            if (auto first_not_space = value.find_first_not_of(' '); first_not_space != std::string_view::npos)
            {
                value.remove_prefix(first_not_space);
            }

            if (auto last_not_space = value.find_last_not_of(' '); last_not_space != std::string_view::npos)
            {
                value.remove_suffix(value.size() - (last_not_space + 1));
            }

            http_request.setHeader(std::string(key), std::string(value));
        }
        pos = end + 2;
    }
}
std::expected<void, RequestParserErr> RequestParser::initBody(int client_fd, char *buffer,
                             const std::string &request_head, std::string &body,
                             size_t request_head_end) const {

    size_t content_length = std::stoul(headers.at("Content-Length"));

    size_t body_start_pos = request_head_end + HeadersEnd.length();

    if (body_start_pos < request_head.length())
    {
        body = request_head.substr(body_start_pos);
    }

    size_t body_received = body.length();

    while (body_received < content_length)
    {

        ssize_t bytes_read = recv(client_fd, buffer,
                                  std::min(static_cast<size_t>(BufferSize), content_length - body_received), 0);

        if (bytes_read < 0)
            return std::unexpected(RequestParserErr::BodyReadErr);
        if (bytes_read == 0)
            break;

        body.append(buffer, bytes_read);
        body_received += bytes_read;
    }
    return {};
}
