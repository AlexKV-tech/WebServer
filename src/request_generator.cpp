#include <filesystem>
#include <iostream>

#include "request_generator.h"

std::string RequestGenerator::generateHttpResponse(
    HttpMethod method, const std::filesystem::path &requested_path,
    const PathForwarder &path_forwarder)
{


    if (auto path = path_forwarder.findServerPath(
        std::make_pair(httpMethodToString(method), requested_path.native())); path.has_value())
    {
        const std::filesystem::path& server_path = path.value();
        auto content_type = path_forwarder.getMimeType(server_path.extension().native());
        std::ifstream fs{server_path, std::ios::in | std::ios::binary};

        std::string html_content{std::istreambuf_iterator<char>(fs),
                                 std::istreambuf_iterator<char>()};

        fs.close();
        return std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: {}\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}",
            content_type, html_content.size(), html_content);
    }



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
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: {}\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}",
        "text/html", err_response.length(), err_response);
}