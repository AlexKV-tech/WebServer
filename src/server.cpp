#include "server.h"

#include <iostream>
#include <stdexcept>
#include <utility>

#include "connection_manager.h"
#include "http_request.h"

Server::Server(int family, int connection_type)
    : connection_manager(family, connection_type), path_forwarder()
{
}

void Server::run()
{
    while (true)
    {
        try
        {
            connection_manager.pollForEvents(path_forwarder);
        }
        catch (std::exception &err)
        {
            std::cerr << err.what();
        }
    }
}

void Server::setPathMapping(HttpMethod method,
                            const std::filesystem::path &requested_path,
                            const std::filesystem::path &response_path)
{
    path_forwarder.addForwardingRule(method, requested_path, response_path);
}

void Server::setPathMapping(const std::map<std::pair<std::string, std::string>,
                                           std::filesystem::path> &routes)
{
    path_forwarder.addForwardingRules(routes);
}
