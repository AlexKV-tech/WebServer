#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>

#include "path_forwarder.hpp"

PathForwarder::PathForwarder(Http::Method method,
                             const std::filesystem::path &requested_path,
                             const std::filesystem::path &response_path)
    : routes({{{std::string(Http::method_to_str(method)), requested_path},
               response_path}}) {}
PathForwarder::PathForwarder()
    : routes({{{"GET", "favicon.ico"}, "static/favicon.ico"}}) {}

std::expected<void, ForwardingErr>
PathForwarder::add_fwd_rule(Http::Method method,
                            const std::filesystem::path &requested_path,
                            const std::filesystem::path &response_path) {
    std::filesystem::path html_path = std::filesystem::absolute(ROOT_FLDR);

    html_path /= response_path;
    if (!std::filesystem::exists(html_path) ||
        !std::filesystem::is_regular_file(html_path)) {
        return std::unexpected(ForwardingErr::IncorrectPathErr);
    }

    auto full_path = std::make_pair(std::string(Http::method_to_str(method)),
                                    requested_path);
    routes[full_path] = html_path;
    return {};
}
void PathForwarder::add_fwd_rules(
    const std::map<std::pair<std::string, std::string>, std::filesystem::path>
        &routes) {
    std::ranges::for_each(routes, [this](const auto &pair) {
        if (!this->add_fwd_rule(Http::str_to_method(pair.first.first),
                                pair.first.second, pair.second))
            std::cerr << "Failed to add forwarding rule" << std::endl;
    });
}
std::optional<std::filesystem::path> PathForwarder::find_server_path(
    const std::pair<std::string_view, std::string_view> &html_path) const {

    if (auto path_map = routes.find(html_path); path_map != routes.end())
        return path_map->second;
    return {};
}
std::string_view PathForwarder::get_mime_type(std::string_view file_extension) {
    if (auto it = mime_types.find(file_extension); it != mime_types.end()) {
        return it->second;
    }
    return "text/html";
}