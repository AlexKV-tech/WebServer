#include <filesystem>

#include "server.hpp"

int main(int argc, char *argv[]) {
    Server s(AF_INET, SOCK_STREAM);
    s.set_path_map(Http::Method::GET, "/", "static/main.html");
    s.set_path_map(Http::Method::GET, "/favicon.ico", "static/favicon.ico");
    s.run();

    return 0;
}