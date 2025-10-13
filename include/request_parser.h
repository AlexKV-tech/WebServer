#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H
#include <sys/socket.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "http_method.h"
#include "http_request.h"

class HttpRequest;
class RequestParser
{
    HttpMethod method;
    std::filesystem::path url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    static constexpr std::string_view ContentLengthTitle = "Content-Length: ";
    static constexpr std::string_view HeadersEnd = "\r\n\r\n";
    static constexpr size_t BufferSize = 16384;

public:
    RequestParser(int client_fd, HttpRequest& http_request);

    void initRequestLine(const std::string& request_head,
                         HttpRequest& http_request);
    void initHeaders(const std::string& request_head,
                     HttpRequest& http_request);
    static void trimCR(std::string& s)
    {
        if (!s.empty() && s.back() == '\r')
            s.pop_back();
    }
    std::pair<ssize_t, size_t> readRequestHead(int client_fd, char* buffer,
                                               std::string& request_head);
    void initBody(int client_fd, char* buffer, std::string& request_head,
                  std::string& body, size_t request_head_end);
};
#endif