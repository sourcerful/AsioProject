#include <iostream>
#include <cstdint>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>

using boost::asio::ip::udp;

std::ostream& operator<<(std::ostream& out, std::vector<uint8_t>& v);   
void read_fileToVector(std::string filePath, std::vector<uint8_t>& vec);
void sendFile(std::vector<uint8_t>& vec, udp::socket& socket, udp::endpoint& server_endpoint);
std::vector<uint8_t> vBuffer;

struct file_info
{
    char fileName[256];
    std::uint32_t size;
};

int main()
{
    std::uint16_t port = 2000;

    file_info fileInfo;
    std::string filePath;
    boost::asio::io_context io_context;
    boost::system::error_code ec;
    udp::endpoint server_endpoint(boost::asio::ip::make_address("127.0.0.1", ec), port);
    std::string protocolInfo;

    std::cout << "Enter the specified file (Full path) to send: ";
    std::cin >> filePath; ///home/sourcer/repos/udpFileTransfer/AsioProject/client/udpclient/src/assets/test.png

    while (!boost::filesystem::exists(filePath))
    {
        std::cerr << "file doesn't exist." << std::endl;
        std::cout << "Enter the specified file (Full path) to send: ";
        std::cin >> filePath;
    }

    if (ec)
        std::cerr << ec.message() << std::endl;
    else
    {
        udp::socket socket(io_context);
        socket.open(udp::v4());

        // used for stream sockets, UDP is datagram-oriented, so using connect is not a must.
        socket.connect(server_endpoint);        

        read_fileToVector(filePath, vBuffer);

        file_info fileInfo;
        
        // Always set the struct values to 0, good practice.
        memset(&fileInfo, 0, sizeof(fileInfo));

        //send file size, name
        fileInfo.size = vBuffer.size();
        strncpy(fileInfo.fileName, filePath.substr(filePath.find_last_of("/\\") + 1).c_str(), 
                  sizeof(fileInfo.fileName) - 1);
        std::cout << "name: " << fileInfo.fileName << std::endl;
        std::cout << "size: " << fileInfo.size << std::endl;
        
        socket.send_to(boost::asio::buffer(&fileInfo, sizeof(fileInfo)), server_endpoint);
        socket.wait(socket.wait_write); // wait for socket to send the data.                        

        try
        {       
           // socket.send_to(boost::asio::buffer(vBuffer.data(), vBuffer.size()), server_endpoint);
           sendFile(vBuffer, socket, server_endpoint);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        socket.close();
    }
}
void sendFile(std::vector<uint8_t>& vec, udp::socket &socket, udp::endpoint& server_endpoint)
{
    std::streampos x;
    uint32_t send = 0;
    int cnt = 0;
    while (send < vec.size()) {
        send += socket.send_to(boost::asio::buffer(vBuffer.data() + send, 
                               std::min(4096U, static_cast<uint32_t>(vBuffer.size() - send))), 
                               server_endpoint);

        std::printf("Sent: %lu\n", send);
        std::printf("cnt=%d\n", cnt++);
    }
}

void read_fileToVector(std::string filePath, std::vector<uint8_t>& vec)
{
    std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    std::streamsize fileSize = file.tellg();
    file.unsetf(std::ios::skipws);
    char byte;
    file.seekg(0, std::ios::beg);
    vec.reserve(fileSize); // maybe resize?

    if (file.is_open())
    {             
        std::copy(std::istream_iterator<uint8_t>(file),
        std::istream_iterator<uint8_t>(),
        std::back_inserter(vec));        
    }
}
std::ostream& operator<<(std::ostream& out, std::vector<char>& v)
{
    for (int i = 0; i < v.size(); i++)
        out << v.at(i);
    return out;
}
