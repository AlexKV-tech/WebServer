#ifndef REQUEST_H
#define REQUEST_H

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>

class HTTPRequest {
    std::string type;
    std::string path;
    std::string headers;
    std::string body;
    static constexpr std::string_view ContentLengthTitle = "Content-Length: ";
    static constexpr std::string_view HeaderEnd = "\r\n\r\n";

public:
    HTTPRequest(const std::string& type, const std::string& path, const std::string& headers, const std::string& body = "");
    std::string getType() const { return type; }
    std::string getPath() const { return type; }
    std::string getHeaders() const { return headers; }
    std::string getBody() const { return body; }
    static std::string parseMethodRequestPath(const std::string& http_request, const std::string& method);
    static unsigned extractContentLength(const std::string& http_request, size_t header_end);
    static std::pair<ssize_t, size_t> readHeader(int client_fd, char* buffer, std::string& request);
    static void readBody(int client_fd, char* buffer, std::string& request, size_t header_end);
};

#endif