#include "server.h"

#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
    Server s(AF_INET, SOCK_STREAM);
    s.setPathMapping("/", "static/main.html");
    s.setPathMapping("/favicon.ico", "static/favicon.ico");
    s.run();

    return 0;
}