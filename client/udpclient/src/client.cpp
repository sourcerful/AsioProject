#include <iostream>
#include <cstdint>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>

using boost::asio::ip::udp;

std::ostream& operator<<(std::ostream& out, std::vector<uint8_t>& v);   
void read_fileToVector(std::string filePath, std::vector<uint8_t>& vec);
void sendFile(udp::socket& socket, udp::endpoint& server_endpoint);
void sendPacket(udp::socket& socket, udp::endpoint& server_endpoint, const uint32_t& packet_size, uint32_t sent, int count, uint8_t times);
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
           sendFile(socket, server_endpoint);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        socket.close();
    }
}
void sendFile(udp::socket &socket, udp::endpoint& server_endpoint)
{
    uint32_t sent = 0;
    uint32_t packet_size = 4096U;
    int count = 0;

    while (sent < vBuffer.size()) //TODO: maybe add a function that gets how many times to send a packet.
    {
		do
		{
        		packet_size = socket.send_to(boost::asio::buffer(vBuffer.data() + sent, 
           	                    std::min(packet_size, static_cast<uint32_t>(vBuffer.size() - sent))), 
           	                    server_endpoint);
		}
		while(packet_size == 0);

		sendPacket(socket, server_endpoint, packet_size, sent, count, 2);	

		sent += packet_size;

		std::cout << "sent: " << sent << std::endl;
        std::cout << "count: " << count++ << std::endl;
    }
}
void sendPacket(udp::socket& socket, udp::endpoint& server_endpoint, const uint32_t& packet_size, uint32_t sent, int count, uint8_t times)
{
    int i = 0;
	uint32_t sendSize = packet_size;	

    while(i < times)
    { 
		sendSize = socket.send_to(boost::asio::buffer(vBuffer.data() + sent, packet_size), server_endpoint); 
		if(sendSize == 0)
			continue;
		else
			i++;
    }
}
void fec_encoder()
{
	;
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

    file.close();
}
std::ostream& operator<<(std::ostream& out, std::vector<char>& v)
{
    for (int i = 0; i < v.size(); i++)
        out << v.at(i);
    return out;
}
