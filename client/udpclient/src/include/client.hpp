#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/array.hpp>
#include "wirehair/wirehair.h"

using boost::asio::ip::udp;

class UDPclient
{
private:
    std::vector<char> _vBuffer;
    std::uint16_t _port;
    boost::asio::io_context _io_context; //io_context - Kind of a I/O service provider. An executor of scheduled tasks (queue).
    std::ofstream _outFile;

public:
    UDPclient(std::uint16_t);
    void run();
    void saveFile(udp::endpoint&, std::uint32_t, udp::socket&, WirehairCodec& decoder);
};

struct file_info
{
    char fileName[256];
    std::uint32_t size;
};

std::ostream& operator<<(std::ostream& out, std::vector<char>& v);
