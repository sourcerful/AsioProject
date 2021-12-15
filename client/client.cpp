#include <iostream>
#include "boost/asio.hpp"
//#include <asio/ts/buffer.hpp>
#include "boost/array.hpp"

using boost::asio::ip::udp;

std::ostream& operator<<(std::ostream& out, std::vector<char> v);

std::vector<char> vBuffer(1024 * 64);

int main()
{
    std::uint16_t port = 5100;
    std::string message = "this message is transported.";

    boost::asio::io_context io_context;
    boost::system::error_code ec;
    udp::endpoint server_endpoint(boost::asio::ip::make_address("127.0.0.1", ec), port);

    if (ec)
        std::cerr << ec.message() << std::endl;
    else
    {
        udp::socket socket(io_context);
        socket.open(udp::v4());
        socket.connect(server_endpoint);

        socket.send_to(boost::asio::buffer(message.data(), message.size()), server_endpoint);

        socket.receive_from(boost::asio::buffer(vBuffer.data(), vBuffer.size()), server_endpoint);
        std::cout << vBuffer << std::endl;
        socket.close();
    }
}

std::ostream& operator<<(std::ostream& out, std::vector<char> v)
{
    for (int i = 0; i < v.size(); i++)
        out << v.at(i);
    return out;
}
