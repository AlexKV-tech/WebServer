#ifndef REQUEST_H
#define REQUEST_H

#include <filesystem>
#include <map>
#include <string>
#include <sys/socket.h>

class HTTPRequest {
    std::string method;
    std::filesystem::path url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    static constexpr std::string_view ContentLengthTitle = "Content-Length: ";
    static constexpr std::string_view HeadersEnd = "\r\n\r\n";
    static constexpr size_t BufferSize = 16384;

public:
    HTTPRequest(const std::string& method, const std::string& path, const std::string& headers, const std::string& body = "");
    std::string getMethod() const { return method; }
    HTTPRequest(const std::string& http_request);
    HTTPRequest(int client_fd);
    std::filesystem::path getPath() const { return url; }

    std::string getBody() const { return body; }

private:
    static void trimCR(std::string& s)
    {
        if (!s.empty() && s.back() == '\r')
            s.pop_back();
    }
    static std::pair<ssize_t, size_t> readRequestHead(int client_fd, char* buffer, std::string& request_head);
    void initBody(int client_fd, char* buffer, std::string& request_head, std::string& body, size_t request_head_end);
    void initRequestLine(const std::string& request_head);
    void initHeaders(const std::string& request_head);
};

#endif