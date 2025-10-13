#include "http_request.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "request_parser.h"

HttpRequest::HttpRequest(int client_fd)
    : method(HttpMethod::NONE), url(""), version(""), body("")

{
    RequestParser(client_fd, *this);
}
void HttpRequest::setMethod(HttpMethod method) { this->method = method; }
void HttpRequest::setUrl(const std::string& url) { this->url = url; }

void HttpRequest::setVersion(const std::string& version)
{
    this->version = version;
}
void HttpRequest::setHeader(const std::string& key, const std::string& value)
{
    this->headers[key] = value;
}
