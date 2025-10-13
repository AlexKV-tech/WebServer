#include "request_parser.h"

RequestParser::RequestParser(int client_fd, HttpRequest& http_request)
{
    std::string request_head;
    char buffer[BufferSize];

    auto [bytes_read, request_head_end] =
        readRequestHead(client_fd, buffer, request_head);

    initRequestLine(request_head, http_request);
    initHeaders(request_head, http_request);

    if (headers.find("Content-Length") != headers.end())
        initBody(client_fd, buffer, request_head, body, request_head_end);
}
std::pair<ssize_t, size_t> RequestParser::readRequestHead(
    int client_fd, char* buffer, std::string& request_head)
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
        throw std::system_error(errno, std::system_category(),
                                "Failed to fetch data from client");
    }
    else if (bytes_read == 0 && request_head.empty())
    {
        std::cerr << "Connection closed by client (fd: " << client_fd << ")"
                  << std::endl;
        throw std::runtime_error(
            "Client closed connection before sending any data");
    }
    return std::make_pair(bytes_read, request_head_end);
}

void RequestParser::initRequestLine(const std::string& request_head,
                                    HttpRequest& http_request)
{
    std::istringstream stream(request_head);
    std::string line;
    if (!std::getline(stream, line))
        throw std::invalid_argument("Empty Http request");

    trimCR(line);

    size_t first_space = line.find(' ');
    size_t second_space = line.find(' ', first_space + 1);

    if (first_space == std::string::npos || second_space == std::string::npos)
        throw std::invalid_argument("Invalid Http request line");

    method = to_http_method(line.substr(0, first_space));
    url = line.substr(first_space + 1, second_space - first_space - 1);
    version = line.substr(second_space + 1);
    http_request.setMethod(method);
    http_request.setUrl(url);
    http_request.setVersion(version);
}
void RequestParser::initHeaders(const std::string& request_head,
                                HttpRequest& http_request)
{
    std::istringstream stream(request_head);
    std::string line;
    while (std::getline(stream, line))
    {
        trimCR(line);
        if (line.empty())
            break;  // End of headers

        auto colon_pos = line.find(':');
        if (colon_pos != std::string::npos)
        {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);

            // Trim leading spaces from value
            size_t start = value.find_first_not_of(' ');
            if (start != std::string::npos)
                value.erase(0, start);

            http_request.setHeader(key, value);
        }
    }
}

void RequestParser::initBody(int client_fd, char* buffer,
                             std::string& request_head, std::string& body,
                             size_t request_head_end)
{
    size_t bytes_read = 0;
    size_t content_length = std::stoul(headers["Content-Length"]);
    size_t body_start = request_head_end + HeadersEnd.length();
    size_t body_received = request_head.length() - body_start;

    while (body_received < content_length)
    {
        bytes_read =
            recv(client_fd, buffer,
                 std::min(sizeof(buffer), content_length - body_received), 0);

        if (bytes_read < 0)
            throw std::system_error(errno, std::system_category(),
                                    "Failed to read request body");
        else if (bytes_read == 0)
            throw std::runtime_error(
                "Client closed connection during body transmission");

        body.append(buffer, bytes_read);
        body_received += bytes_read;
    }
}
