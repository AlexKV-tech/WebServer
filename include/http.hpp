#ifndef HTTP_HPP
#define HTTP_HPP

#include <expected>
#include <filesystem>
#include <string>

#include "http_types.hpp"
#include "path_forwarder.hpp"

namespace Http {
std::string generate_response(Method method,
                              const std::filesystem::path &requested_path,
                              const PathForwarder &path_forwarder);
std::expected<Request, ParseErr> parse(int fd, Request &http_request);
} // namespace Http

#endif