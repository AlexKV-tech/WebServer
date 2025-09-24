#include "server.h"

#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
    Server s(AF_INET, SOCK_STREAM);
    s.setPathMapping("GET", "/", "static/main.html");
    s.setPathMapping("GET", "/favicon.ico", "static/favicon.ico");
    s.run();

    return 0;
}