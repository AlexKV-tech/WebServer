#ifndef PATH_FORWARD_H
#define PATH_FORWARD_H

#include "request.h"

#include <filesystem>
#include <map>
#include <string>

class PathForwarder {
    inline static const std::map<std::string, std::string>
        mime_types = {
            // Text / Web
            { ".html", "text/html" },
            { ".htm", "text/html" },
            { ".css", "text/css" },
            { ".js", "application/javascript" },
            { ".json", "application/json" },
            { ".xml", "application/xml" },

            // Images
            { ".png", "image/png" },
            { ".jpg", "image/jpeg" },
            { ".jpeg", "image/jpeg" },
            { ".gif", "image/gif" },
            { ".svg", "image/svg+xml" },
            { ".webp", "image/webp" },
            { ".ico", "image/x-icon" },
            { ".bmp", "image/bmp" },
            { ".tiff", "image/tiff" },
            { ".avif", "image/avif" },

            // Fonts
            { ".woff", "font/woff" },
            { ".woff2", "font/woff2" },
            { ".ttf", "font/ttf" },
            { ".otf", "font/otf" },
            { ".eot", "application/vnd.ms-fontobject" },

            // Audio / Video
            { ".mp3", "audio/mpeg" },
            { ".wav", "audio/wav" },
            { ".ogg", "audio/ogg" },
            { ".mp4", "video/mp4" },
            { ".webm", "video/webm" },
            { ".ogg", "video/ogg" }
        };

    std::map<std::pair<std::string, std::string>, std::filesystem::path> routes;

public:
    PathForwarder(HTTPRequest::HTTPMethod method, const std::filesystem::path& requested_path,
        const std::filesystem::path& response_path);
    PathForwarder();
    std::string
    generateHttpResponse(HTTPRequest::HTTPMethod method, const std::filesystem::path& requested_path) const;
    void addForwardingRule(HTTPRequest::HTTPMethod method, const std::filesystem::path& requested_path,
        const std::filesystem::path& response_path);
    void addForwardingRules(
        const std::map<std::pair<std::string, std::string>, std::filesystem::path>& routes);
    static std::string getContentType(HTTPRequest::HTTPMethod method, const std::filesystem::path& path);
};

#endif