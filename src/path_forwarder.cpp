#include "path_forwarder.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>

PathForwarder::PathForwarder(HttpMethod method,
                             const std::filesystem::path& requested_address,
                             const std::filesystem::path& response_path)
    : routes({{{to_string(method), requested_address}, response_path}})
{
}
PathForwarder::PathForwarder()
    : routes({{{"GET", "favicon.ico"}, "static/favicon.ico"}})
{
}

void PathForwarder::addForwardingRule(
    HttpMethod method, const std::filesystem::path& requested_path,
    const std::filesystem::path& response_path)
{
    std::filesystem::path html_path = std::filesystem::absolute(ROOT_FLDR);

    html_path /= response_path;
    if (!std::filesystem::exists(html_path) ||
        !std::filesystem::is_regular_file(html_path))
        throw std::invalid_argument(
            "Either incorrect path, path without file or path to "
            "non-html file was provided");
    auto full_path = std::make_pair(to_string(method), requested_path);
    routes[full_path] = html_path;
}
void PathForwarder::addForwardingRules(
    const std::map<std::pair<std::string, std::string>, std::filesystem::path>&
        routes)
{
    std::for_each(routes.begin(), routes.end(),
                  [this](const auto& pair)
                  {
                      this->addForwardingRule(to_http_method(pair.first.first),
                                              pair.first.second, pair.second);
                  });
}
std::optional<std::filesystem::path> PathForwarder::findServerPath(
    const std::pair<std::string, std::string>& html_path) const
{
    auto path_map = routes.find(html_path);
    assert(routes.size() >= 2);
    if (path_map == routes.end())
        return {};
    return path_map->second;
}
std::string PathForwarder::getMimeType(const std::string file_extension) const
{
    return mime_types.at(file_extension);
}