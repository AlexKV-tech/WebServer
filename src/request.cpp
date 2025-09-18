#include "request.h"

std::string HTTPRequest::parseMethodRequestPath(const std::string& http_request, const std::string& method)
{

    size_t method_pos = http_request.find(method);
    if (method_pos == std::string::npos)
        return "";

    size_t path_start = method_pos + method.length();
    if (!http_request.empty() && method.back() != ' ')
        path_start++;
    size_t path_end = http_request.find(' ', path_start);
    if (path_end == std::string::npos)
        return "";

    std::string path = http_request.substr(path_start, path_end - path_start);

    size_t last_slash = path.find_last_of('/');
    path.erase(std::remove_if(path.begin(), path.end(), isspace), path.end());
    if (last_slash != std::string::npos && last_slash + 1 < path.size()) {
        return path.substr(last_slash + 1);
    }
    return path;
}

unsigned HTTPRequest::extractContentLength(const std::string& http_request, size_t header_end)
{

    size_t cl_pos = http_request.find(ContentLengthTitle);
    if (cl_pos != std::string::npos && cl_pos < header_end) {
        cl_pos += ContentLengthTitle.length(); // Move past "Content-Length: "
        size_t cl_end = http_request.find("\r\n", cl_pos);
        if (cl_end != std::string::npos) {
            try {
                std::string cl_str = http_request.substr(cl_pos, cl_end - cl_pos);
                return std::stoul(cl_str);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid Content-Length header" << std::endl;
            }
        }
    }
    return 0;
}
std::pair<ssize_t, size_t> HTTPRequest::readHeader(int client_fd, char* buffer, std::string& request)
{

    ssize_t bytes_read;
    size_t header_end = std::string::npos;

    // Phase 1: Read until we get all headers (until \r\n\r\n)
    while ((bytes_read = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        request.append(buffer, bytes_read);

        if ((header_end = request.find(HeaderEnd)) != std::string::npos)
            break; // Found end of headers
    }
    return std::make_pair(bytes_read, header_end);
}
void HTTPRequest::readBody(int client_fd, char* buffer, std::string& request, size_t header_end)
{
    size_t bytes_read = 0;
    size_t content_length = extractContentLength(request, header_end);
    size_t body_start = header_end + HeaderEnd.length();
    size_t body_received = request.length() - body_start;

    while (body_received < content_length) {
        bytes_read = recv(client_fd, buffer,
            std::min(sizeof(buffer), content_length - body_received), 0);

        if (bytes_read < 0)
            throw std::system_error(errno, std::system_category(),
                "Failed to read request body");
        else if (bytes_read == 0)
            throw std::runtime_error("Client closed connection during body transmission");

        request.append(buffer, bytes_read);
        body_received += bytes_read;
    }
}