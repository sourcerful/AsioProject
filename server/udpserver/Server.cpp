#include "Server.hpp"

UDPServer::UDPServer(std::uint16_t port) : _port(port)
{
    udp::endpoint endpoint(boost::asio::ip::udp::v4(), _port); //recieves IPv4 connections.
    this->_reciever = endpoint;
}

void UDPServer::run()
{
    std::thread thread_context = std::thread([&] {_io_context.run(); }); //start the context's work.
    //Give fake tasks so context doesn't finish. this is for async use
    boost::asio::io_context::work idleWork(_io_context);
    CHAR NPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, NPath);

    std::printf("Working Dir: %s\n", NPath);

    udp::socket socket(_io_context, _reciever); //the file descriptor / socket descriptor.
    boost::system::error_code error;

    std::cout << "listening." << std::endl;

    udp::endpoint sender;
    file_info fileInfo;
    memset(&fileInfo, sizeof(fileInfo), 0);

    //socket.recieve_from waits for an incoming packet (UDP at this case), fills the buffer with the recieved data
    //and fills the "sender" endpoint with an information about the sender which is also a pair of IP and port. 

    std::size_t bytes_transferred = socket.receive_from(
        boost::asio::buffer(&fileInfo, sizeof(fileInfo)),
        sender,
        0,
        error);

    std::cout << "connection from " << sender.address() << std::endl;
    std::cout << fileInfo.fileName << std::endl;
    std::cout << "size: " << fileInfo.size << std::endl;

    _vBuffer.resize(4096);

    std::cout << "file: " << fileInfo.fileName << std::endl << "size: " << fileInfo.size << std::endl;

    //several overloads to recieve_from and send_to.
    //socket.send_to(boost::asio::buffer(vBuffer.data(), bytes_transferred), sender);            
    _outFile.open(fileInfo.fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    saveFile(sender, fileInfo.size, socket);

    std::cout << "file saved" << std::endl;

    if (thread_context.joinable())
        thread_context.join();    

    std::cout << "I'm done!" << std::endl;
}
// three / is good
void UDPServer::saveFile(udp::endpoint& sender, std::uint32_t maxFileSize, udp::socket& socket)
{
    socket.async_receive_from(boost::asio::buffer(_vBuffer.data(), _vBuffer.size()), sender, 0, 
        [&sender, &socket, maxFileSize, this](boost::system::error_code ec, std::size_t length)
        {     
            if (ec) {
                std::cerr << ec.message() << std::endl;
                return;
            }
            try
            {
                std::printf("mike: Got %lu size, writing..\n", length);
                _outFile.write(_vBuffer.data(), length);

                std::cout << "REACHED POINTER= " << _outFile.tellp() << std::endl;
                std::cout << "max file size: " << maxFileSize << std::endl;
                if (_outFile.tellp() == maxFileSize) {
                    std::printf("Stopping!, max file size=%lu, ptr= %lu\n", maxFileSize, _outFile.tellp());
                    _io_context.stop();
                    return;
                }
            }
            catch (std::exception& ex)
            {
                std::cout << ex.what() << std::endl;
                _io_context.stop();
                return;
            }

            saveFile(sender, maxFileSize, socket);            
        }
    );    
}
std::ostream& operator<<(std::ostream& out, std::vector<BYTE>& v)
{
    for (int i = 0; i < v.size(); i++)
        out << " " << v.at(i);
   
    return out;
}
