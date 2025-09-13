#include <filesystem>
#include <fstream>

#include "server.h"

int main(int argc, char* argv[])
{

    std::filesystem::current_path(ROOT_FLDR);

    Server s(AF_INET, SOCK_STREAM);
    s.setStaticFilesForwarding("/", "static/main.html");
    s.setStaticFilesForwarding("/favicon.ico", "static/favicon.ico");
    s.run();

    return 0;
}