#include "path_forward.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>

PathForwarder::PathForwarder(HTTPRequest::HTTPMethod method, const std::filesystem::path& requested_address,
    const std::filesystem::path& response_path)
    : routes({ { { HTTPRequest::to_string(method), requested_address }, response_path } })
{
}
PathForwarder::PathForwarder()
    : routes({ { { "GET", "favicon.ico" }, "static/favicon.ico" } })
{
}
std::string PathForwarder::generateHttpResponse(HTTPRequest::HTTPMethod method,
    const std::filesystem::path& requested_path) const
{
    auto full_path = std::make_pair(HTTPRequest::to_string(method), requested_path);
    if (routes.find(full_path) == routes.end()) {
        std::string err_response = std::format(
            "<html>"
            "<head><title> 404 Not Found</title></head>"
            "<body>"
            "<h1>Not Found</ h1>"
            "<p>The requested URL/ {} was not found on this server.</p>"
            "</body>"
            "</html>",
            requested_path.string());
        return std::format("HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: {}\r\n"
                           "Content-Length: {}\r\n"
                           "\r\n"
                           "{}",
            "text/html", err_response.length(), err_response);
    }
    std::filesystem::path html_path = routes.at(full_path);
    auto content_type = mime_types.at(html_path.extension());
    std::ifstream fs(html_path, std::ios::in | std::ios::binary);

    std::string html_content((std::istreambuf_iterator<char>(fs)),
        std::istreambuf_iterator<char>());

    fs.close();
    return std::format("HTTP/1.1 200 OK\r\n"
                       "Content-Type: {}\r\n"
                       "Content-Length: {}\r\n"
                       "\r\n"
                       "{}",
        content_type, html_content.size(), html_content);
}
void PathForwarder::addForwardingRule(
    HTTPRequest::HTTPMethod method, const std::filesystem::path& requested_path,
    const std::filesystem::path& response_path)
{
    std::filesystem::path html_path = std::filesystem::absolute(ROOT_FLDR);

    html_path /= response_path;
    if (!std::filesystem::exists(html_path) || !std::filesystem::is_regular_file(html_path))
        throw std::invalid_argument(
            "Either incorrect path, path without file or path to "
            "non-html file was provided");
    auto full_path = std::make_pair(HTTPRequest::to_string(method), requested_path);
    routes[full_path] = html_path;
}
void PathForwarder::addForwardingRules(
    const std::map<std::pair<std::string, std::string>, std::filesystem::path>& routes)
{
    std::for_each(routes.begin(), routes.end(), [this](const auto& pair) {
        this->addForwardingRule(HTTPRequest::to_http_method(pair.first.first), pair.first.second, pair.second);
    });
}