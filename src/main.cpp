#include <filesystem>
#include <iostream>

#include "server.h"

int main(int argc, char* argv[])
{
    Server s(AF_INET, SOCK_STREAM);
    s.setPathMapping(HttpMethod::GET, "/", "static/main.html");
    s.setPathMapping(HttpMethod::GET, "/favicon.ico", "static/favicon.ico");
    s.run();

    return 0;
}