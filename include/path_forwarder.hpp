#ifndef PATH_FORWARDER_HPP
#define PATH_FORWARDER_HPP

#include <expected>
#include <filesystem>
#include <map>
#include <optional>
#include <string>

#include "http_types.hpp"

enum class ForwardingErr {
    IncorrectPathErr,

};

class PathForwarder {
    inline static const std::map<std::string, std::string, std::less<>>
        mime_types = {
            // Text / Web
            {".html", "text/html"},
            {".htm", "text/html"},
            {".css", "text/css"},
            {".js", "application/javascript"},
            {".json", "application/json"},
            {".xml", "application/xml"},

            // Images
            {".png", "image/png"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".gif", "image/gif"},
            {".svg", "image/svg+xml"},
            {".webp", "image/webp"},
            {".ico", "image/x-icon"},
            {".bmp", "image/bmp"},
            {".tiff", "image/tiff"},
            {".avif", "image/avif"},

            // Fonts
            {".woff", "font/woff"},
            {".woff2", "font/woff2"},
            {".ttf", "font/ttf"},
            {".otf", "font/otf"},
            {".eot", "application/vnd.ms-fontobject"},

            // Audio / Video
            {".mp3", "audio/mpeg"},
            {".wav", "audio/wav"},
            {".ogg", "audio/ogg"},
            {".mp4", "video/mp4"},
            {".webm", "video/webm"},
            {".ogg", "video/ogg"}};

    std::map<std::pair<std::string, std::string>, std::filesystem::path,
             std::less<>>
        routes;

  public:
    PathForwarder(Http::Method method,
                  const std::filesystem::path &requested_path,
                  const std::filesystem::path &response_path);
    PathForwarder();

    std::expected<void, ForwardingErr>
    add_fwd_rule(Http::Method method,
                 const std::filesystem::path &requested_path,
                 const std::filesystem::path &response_path);
    void add_fwd_rules(const std::map<std::pair<std::string, std::string>,
                                      std::filesystem::path> &routes);
    static std::string_view getContentType(Http::Method method,
                                           const std::filesystem::path &path);
    [[nodiscard]] std::optional<std::filesystem::path> find_server_path(
        const std::pair<std::string_view, std::string_view> &html_path) const;
    [[nodiscard]] static std::string_view
    get_mime_type(std::string_view file_extension);
};

#endif