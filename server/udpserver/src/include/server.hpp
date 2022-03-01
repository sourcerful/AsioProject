#ifndef SERVER_F // This file is for later use.
#define SERVER_F

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include "wirehair/wirehair.h"

struct file_info
{
    char fileName[256];
    std::uint32_t size;
};

using boost::asio::ip::udp;

class UDPserver
{
    std::uint16_t port;
	std::vector<uint8_t> vBuffer;
	boost::asio::io_context io_context;

public:
    UDPserver(uint16_t port);
    void run();
    void sendFile(udp::socket& socket, udp::endpoint& client_endpoint, file_info& fileInfo);
	void sendFileInfo(file_info& fileInfo, std::string filePath, udp::socket& socket, udp::endpoint& client_endpoint);
	uint32_t create_encoder(WirehairCodec& encoder, std::uint32_t fileSize);
	void read_fileToVector(std::string filePath);
};

std::ostream& operator<<(std::ostream& out, std::vector<uint8_t>& vec);

#endif
