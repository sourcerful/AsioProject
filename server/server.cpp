#include <iostream>
#include <boost/asio.hpp>
//#include <asio/ts/buffer.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp;

std::vector<char> vBuffer(64 * 1024);

int main()
{
    std::uint16_t port = 5100;

    boost::asio::io_context io_context; // Kind of a I/O service provider. An executor of scheduled tasks.
    udp::endpoint reciever(boost::asio::ip::udp::v4(), port); //recieves IPv4 connections.
    udp::socket socket(io_context, reciever); //the file descriptor / socket descriptor.

    std::cout << "listening." << std::endl;

    for (;;)
    {
        //endpoint of the sender.
        udp::endpoint sender;
        //socket.recieve_from waits for an incoming packet (UDP at this case), fills the buffer with the recieved data
        //and fills the "sender" endpoint with an information about the sender which is also a pair of IP and port. 
        std::size_t bytes_transferred = socket.receive_from(boost::asio::buffer(vBuffer.data(), vBuffer.size()), sender);
        std::cout << "connection from " << sender.address() << std::endl;
        //several overloads to recieve_from and send_to.
        socket.send_to(boost::asio::buffer(vBuffer.data(), bytes_transferred), sender);

    }
}
