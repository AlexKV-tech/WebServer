#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <filesystem>
#include <map>
#include <string>

#include "http_method.h"

class HttpRequest
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
    HttpRequest(HttpMethod method, std::string_view path,
                std::string_view headers, std::string_view body = "");
    [[nodiscard]] HttpMethod getMethod() const { return method; }
    HttpRequest(std::string_view http_request);

    explicit HttpRequest(int client_fd);
    [[nodiscard]] std::filesystem::path getPath() const { return url; }
    void setMethod(HttpMethod method);
    void setUrl(std::string_view url);
    void setVersion(std::string_view version);
    void setHeader(std::string_view key, std::string_view value);

    [[nodiscard]] std::string getBody() const { return body; }




};

#endif