#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <expected>

#include "http_method.h"
#include "http_request.h"

enum class RequestParserErr {
    FetchErr,
    PrematureDisconnectionErr,
    BodyReadErr,
    InvalidHttpErr,
};

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


private:
    static std::expected<void, RequestParserErr> initRequestLine(std::string_view request_head,
                                                                 HttpRequest& http_request);
    static void initHeaders(std::string_view request_head,
                            HttpRequest& http_request);
    static void trimCR(std::string& s)
    {
        if (!s.empty() && s.back() == '\r')
            s.pop_back();
    }
    static std::expected<std::pair<ssize_t, size_t>, RequestParserErr> readRequestHead(int client_fd, char* buffer,
                                                                                       std::string& request_head);
    std::expected<void, RequestParserErr> initBody(int client_fd, char* buffer, const std::string& request_head,
                  std::string& body, size_t request_head_end) const;

};
#endif