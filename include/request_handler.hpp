#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <expected>

#include "path_forwarder.hpp"

enum class RequestHandlerErr {
    SendErr,
};

namespace RequestHandler {

std::expected<void, RequestHandlerErr> handle(int fd, const PathForwarder &pf);
}

#endif