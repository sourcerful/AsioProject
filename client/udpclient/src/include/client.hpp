#ifndef CLIENT_F // This file is for later use.
#define CLIENT_F

#include <iostream>
#include <cstdint>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>

struct file_info
{
    char fileName[256];
    std::uint32_t size;
};

class UDPclient
{
    std::uint16_t port;
    file_info fileInfo;

    UDPclient();
    void run();
    ~UDPclient();
};

#endif