#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <sys/socket.h>

#include <filesystem>
#include <map>
#include <string>

#include "http_method.h"

class HttpRequest
{
public:
public:
    HttpRequest(HttpMethod method, const std::string& path,
                const std::string& headers, const std::string& body = "");
    HttpMethod getMethod() const { return method; }
    HttpRequest(const std::string& http_request);
    HttpRequest(int client_fd);
    std::filesystem::path getPath() const { return url; }
    void setMethod(HttpMethod method);
    void setUrl(const std::string& url);
    void setVersion(const std::string& version);
    void setHeader(const std::string& key, const std::string& value);

    std::string getBody() const { return body; }

private:
    HttpMethod method;
    std::filesystem::path url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    static constexpr std::string_view ContentLengthTitle = "Content-Length: ";
    static constexpr std::string_view HeadersEnd = "\r\n\r\n";
    static constexpr size_t BufferSize = 16384;
};

#endif