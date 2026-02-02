#include <iostream>
#include <utility>

#include "server.hpp"

Server::Server(int family, int connection_type)
    : connection_manager(family, connection_type), path_forwarder() {}

void Server::run() {
    while (true) {
        if (!connection_manager.pollForEvents(path_forwarder)) {
            std::cerr << "Failed to poll for events" << std::endl;
            break;
        }
    }
}

void Server::set_path_map(Http::Method method,
                          const std::filesystem::path &requested_path,
                          const std::filesystem::path &response_path) {
    if (!path_forwarder.add_fwd_rule(method, requested_path, response_path))
        std::cerr << "Failed to add forwarding rule" << std::endl;
}

void Server::set_path_map(const std::map<std::pair<std::string, std::string>,
                                         std::filesystem::path> &routes) {
    path_forwarder.add_fwd_rules(routes);
}
