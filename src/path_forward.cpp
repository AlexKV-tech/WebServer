#include "path_forward.h"

PathForwarder::PathForwarder(const std::string& requested_address,
    const std::string& response_path)
    : routes({ { requested_address, response_path } })
{
}
PathForwarder::PathForwarder()
    : routes({ { "/favicon.ico", "static/favicon.ico" } })
{
}
std::string PathForwarder::generateHttpResponse(const std::string& requested_path) const
{
    if (routes.find(requested_path) == routes.end()) {
        return std::format(
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: {}\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "<html>"
            "<head><title> 404 Not Found</title></head>"
            "<body>"
            "<h1>Not Found</ h1>"
            "<p>The requested URL/ {} was not found on this server.</p>"
            "</body>"
            "</html>",
            "text/html",
            210,
            requested_path);
    }
    std::filesystem::path html_path = routes.at(requested_path);
    auto content_type = mime_types.at(html_path.extension());
    std::ifstream fs(html_path, std::ios::in | std::ios::binary);

    std::string html_content((std::istreambuf_iterator<char>(fs)),
        std::istreambuf_iterator<char>());

    fs.close();
    return std::format(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: {}\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}",
        content_type,
        html_content.size(),
        html_content);
}
void PathForwarder::addForwardingRule(const std::string& requested_path,
    const std::string& response_path)
{
    std::filesystem::path html_path = std::filesystem::absolute(ROOT_FLDR);
    html_path /= response_path;
    if (!std::filesystem::exists(html_path) || !std::filesystem::is_regular_file(html_path))
        throw std::runtime_error("Either incorrect path, path without file or path to "
                                 "non-html file was provided");

    routes[requested_path] = response_path;
}
void PathForwarder::addForwardingRules(const std::map<std::string, std::string>& routes)
{
    std::for_each(routes.begin(), routes.end(), [this](const auto& pair) { this->addForwardingRule(pair.first, pair.second); });
}