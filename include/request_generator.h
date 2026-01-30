#ifndef REQUEST_GENERATOR_H
#define REQUEST_GENERATOR_H

#include <filesystem>
#include <fstream>

#include "http_method.h"
#include "path_forwarder.h"

class RequestGenerator

{
public:
    static std::string generateHttpResponse(
        HttpMethod method, const std::filesystem::path& requested_path,
        const PathForwarder& path_forwarder);
};

#endif