#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <expected>
#include <iostream>

#include "connection_manager.hpp"
#include "request_handler.hpp"

ConnectionManager::ConnectionManager(int family, int connection_type)
    : family(family), connection_type(connection_type),
      listener(std::make_unique<Listener>(family, connection_type)) {
    listener->bind_addr(gen_local_addr());
    listener->listen_conns(Connections);
}

sockaddr_in ConnectionManager::gen_local_addr() const {
    sockaddr_in address{};
    memset(&address, 0, sizeof(address));

    address.sin_family = family;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(ListenerPort);

    return address;
}

void ConnectionManager::log_conn(const sockaddr_in &client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr.sin_port);
    std::cout << "New connection from " << client_ip << ":" << client_port
              << std::endl;
}

std::expected<void, ConnectionManagerErr>
ConnectionManager::poll_for_events(const PathForwarder &path_forwarder) {
    size_t fds_cnt = client_sockets.size() + 1;


    pollfd fds[fds_cnt];

    fds[0] = listener->get_poll_cfg(); // set listener pollfd

    for (size_t i = 1; i < fds_cnt; i++) {
        // client_sockets contains one less socket than the actual number of fds due to listener
        fds[i] = {client_sockets[i - 1]->getFd(), POLLIN, 0}; 
    }

    int pending = poll(fds, fds_cnt, 100);

    if (pending < 0) {
        return std::unexpected(ConnectionManagerErr::PollErr);
    }

    if (pending == 0) {
        std::cout << "No connections pending" << std::endl;
        return {};
    }

    std::vector<size_t> to_remove;

    if (fds[0].revents & POLLIN) {
        if (!accept_conn())
            return std::unexpected(ConnectionManagerErr::acceptErr);
    }

    for (size_t i = 1; i < fds_cnt; ++i) {
        if (fds[i].revents == 0)
            continue;

        size_t client_idx = i - 1;

        if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
            to_remove.push_back(client_idx);
        } else if (fds[i].revents & POLLIN) {
            if (!RequestHandler::handle(client_sockets[client_idx]->getFd(),
                                        path_forwarder))
                to_remove.push_back(client_idx);
        }
    }

    if (!to_remove.empty()) {
        std::sort(to_remove.rbegin(), to_remove.rend());
        for (size_t idx : to_remove) {
            client_sockets.erase(client_sockets.begin() + idx);
        }
    }
    return {};
}

std::expected<void, ConnectionManagerErr>
ConnectionManager::accept_conn() {
    sockaddr_in client_addr{};
    if (auto accept_result = listener->accept_conn(client_addr);
        accept_result.has_value()) {
        client_sockets.push_back(std::make_unique<Socket>(
            family, connection_type, accept_result.value()));

        log_conn(client_addr);
        return {};
    }
    return std::unexpected(ConnectionManagerErr::acceptErr);
}