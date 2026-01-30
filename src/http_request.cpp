#include "http_request.h"
#include "request_parser.h"

HttpRequest::HttpRequest(int client_fd)
    : method(HttpMethod::NONE), url(""), version(""), body("")

{
    RequestParser(client_fd, *this);
}
void HttpRequest::setMethod(HttpMethod method) { this->method = method; }
void HttpRequest::setUrl(std::string_view url) { this->url = url; }

void HttpRequest::setVersion(std::string_view version)
{
    this->version = version;
}
void HttpRequest::setHeader(std::string_view key, std::string_view value)
{
    this->headers[std::string(key)] = value;
}
