#include "include/server.hpp"

using boost::asio::ip::udp;

int main()
{
    std::uint16_t port = 2000;

    UDPServer server(port);
    server.run();
}
