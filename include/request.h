#ifndef REQUEST_H
#define REQUEST_H

#include <filesystem>
#include <map>
#include <string>
#include <sys/socket.h>

class HTTPRequest {
public:
    enum class HTTPMethod {
        GET,
        POST,
        PUT,
        DELETE,
        NONE
    };
    static std::string to_string(HTTPMethod method)
    {
        switch (method) {
        case HTTPMethod::GET:
            return "GET";
        case HTTPMethod::POST:
            return "POST";
        case HTTPMethod::PUT:
            return "PUT";
        case HTTPMethod::DELETE:
            return "DELETE";
        default:
            return "";
        }
    }
    static HTTPMethod to_http_method(const std::string& method)
    {
        if (method == "GET")
            return HTTPMethod::GET;
        else if (method == "POST")
            return HTTPMethod::POST;
        else if (method == "PUT")
            return HTTPMethod::PUT;
        else if (method == "DELETE")
            return HTTPMethod::DELETE;
        return HTTPMethod::NONE;
    }

public:
    HTTPRequest(HTTPMethod method, const std::string& path, const std::string& headers, const std::string& body = "");
    HTTPMethod getMethod() const { return method; }
    HTTPRequest(const std::string& http_request);
    HTTPRequest(int client_fd);
    std::filesystem::path getPath() const { return url; }

    std::string getBody() const { return body; }

private:
    HTTPMethod method;
    std::filesystem::path url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    static constexpr std::string_view ContentLengthTitle = "Content-Length: ";
    static constexpr std::string_view HeadersEnd = "\r\n\r\n";
    static constexpr size_t BufferSize = 16384;

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