#include "include/server.hpp"

using boost::asio::ip::udp;

std::vector<char> vBuffer(64 * 1024);

int main()
{
    std::uint16_t port = 2000;

    UDPServer server(port);
    server.run();
}
