#include "request_generator.h"

#include <filesystem>

std::string RequestGenerator::generateHttpResponse(
    HttpMethod method, const std::filesystem::path& requested_path,
    const PathForwarder& path_forwarder)
{
    auto path = path_forwarder.findServerPath(
        std::make_pair(to_string(method), requested_path));
    std::cout << "path: " << path.has_value() << '\n';
    if (!path.has_value())
    {
        std::string err_response = std::format(
            "<html>"
            "<head><title> 404 Not Found</title></head>"
            "<body>"
            "<h1>Not Found</ h1>"
            "<p>The requested URL/ {} was not found on this server.</p>"
            "</body>"
            "</html>",
            requested_path.string());
        return std::format(
            "Http/1.1 404 Not Found\r\n"
            "Content-Type: {}\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}",
            "text/html", err_response.length(), err_response);
    }
    std::filesystem::path server_path = *path;
    auto content_type = path_forwarder.getMimeType(server_path.extension());
    std::ifstream fs(server_path, std::ios::in | std::ios::binary);

    std::string html_content((std::istreambuf_iterator<char>(fs)),
                             std::istreambuf_iterator<char>());

    fs.close();
    return std::format(
        "Http/1.1 200 OK\r\n"
        "Content-Type: {}\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}",
        content_type, html_content.size(), html_content);
}