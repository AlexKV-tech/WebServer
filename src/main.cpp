#include "server.h"

#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
    Server s(AF_INET, SOCK_STREAM);
    s.setPathMapping(HTTPRequest::HTTPMethod::GET, "/", "static/main.html");
    s.setPathMapping(HTTPRequest::HTTPMethod::GET, "/favicon.ico", "static/favicon.ico");
    s.run();

    return 0;
}