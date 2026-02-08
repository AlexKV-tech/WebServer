#ifndef HTTP_TYPES_HPP
#define HTTP_TYPES_HPP
#include <filesystem>
#include <map>
#include <string>

namespace Http {
enum class Method { GET, POST, PUT, DELETE, NONE };
enum class ParseErr {
    FetchErr,
    PrematureDisconnectionErr,
    BodyReadErr,
    InvalidHttpErr,
};
constexpr std::string_view method_to_str(Method method) {
    switch (method) {
    case Method::GET:
        return "GET";
    case Method::POST:
        return "POST";
    case Method::PUT:
        return "PUT";
    case Method::DELETE:
        return "DELETE";
    default:
        return "UNKNOWN";
    }
}
inline Method str_to_method(std::string_view method) {
    if (method == "GET")
        return Method::GET;
    else if (method == "POST")
        return Method::POST;
    else if (method == "PUT")
        return Method::PUT;
    else if (method == "DELETE")
        return Method::DELETE;
    return Method::NONE;
}

struct Request {
    Method method;
    std::filesystem::path url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    static constexpr std::string_view CONTENT_LEN_TITLE = "Content-Length: ";
    static constexpr std::string_view HEADERS_END = "\r\n\r\n";
    static constexpr std::string_view HEADERS_SPACE = "\r\n";
    static constexpr size_t BUF_SIZE = 16384;
};
} // namespace Http

#endif