#ifndef PATH_FORWARDER_H
#define PATH_FORWARDER_H

#include <filesystem>
#include <map>
#include <optional>
#include <string>

#include "http_method.h"

class PathForwarder
{
    inline static const std::map<std::string, std::string> mime_types = {
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

    std::map<std::pair<std::string, std::string>, std::filesystem::path>
        routes;

public:
    PathForwarder(HttpMethod method,
                  const std::filesystem::path& requested_path,
                  const std::filesystem::path& response_path);
    PathForwarder();

    void addForwardingRule(HttpMethod method,
                           const std::filesystem::path& requested_path,
                           const std::filesystem::path& response_path);
    void addForwardingRules(const std::map<std::pair<std::string, std::string>,
                                           std::filesystem::path>& routes);
    static std::string getContentType(HttpMethod method,
                                      const std::filesystem::path& path);
    std::optional<std::filesystem::path> findServerPath(
        const std::pair<std::string, std::string>& html_path) const;
    std::string getMimeType(const std::string file_extension) const;
};

#endif