#include "include/server.hpp"

using boost::asio::ip::udp;

UDPserver::UDPserver(uint16_t port) 
{
	this->port = port;
}

void UDPserver::run()
{
    file_info fileInfo;
    std::string filePath;

	udp::socket socket(io_context); 
	udp::endpoint destination(boost::asio::ip::address::from_string("127.0.0.1"), port);
	socket.open(udp::v4());
	
	boost::system::error_code ec;
	const WirehairResult initResult = wirehair_init(); //initialize wirehair

	if(initResult != Wirehair_Success)
	{
		std::cout << "failed to initialize wirehair: " << initResult << std::endl;
		return;
	}

    if (ec)
        std::cerr << ec.message() << std::endl;
    else
    { 
		sendFileInfo(fileInfo, filePath, socket, destination);

		try
        {       
           sendFile(socket, destination, fileInfo);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
		
		std::cout << "File uploaded, closing." << std::endl;
       
		socket.close();
    }
}
void UDPserver::sendFileInfo(file_info& fileInfo, std::string filePath, udp::socket& socket, udp::endpoint& client_endpoint)
{
	
		std::cout << "Enter the specified file (Full path) to send: ";
		std::cin >> filePath; 

		while (!boost::filesystem::exists(filePath))
		{
			std::cerr << "file doesn't exist." << std::endl;
			std::cout << "Enter the specified file (Full path) to send: ";
			std::cin >> filePath;
		}

		read_fileToVector(filePath);
        
		memset(&fileInfo, 0, sizeof(fileInfo)); // Always set the struct values to 0, good practice.
        //send file size, name
        fileInfo.size = vBuffer.size();
        strncpy(fileInfo.fileName, filePath.substr(filePath.find_last_of("/\\") + 1).c_str(), 
                  sizeof(fileInfo.fileName) - 1);
        std::cout << "name: " << fileInfo.fileName << std::endl;
        std::cout << "size: " << fileInfo.size << std::endl;

        socket.send_to(boost::asio::buffer(&fileInfo, sizeof(fileInfo)), client_endpoint);
        socket.wait(socket.wait_write);

}
void UDPserver::sendFile(udp::socket &socket, udp::endpoint& client_endpoint, file_info& fileInfo)
{
	unsigned int blockID = 1;	
	WirehairCodec encoder;	
    uint32_t sent = 0;	
	uint32_t kPacketSize = create_encoder(encoder, fileInfo.size); 

    while (sent < fileInfo.size)
	{
		std::vector<uint8_t> block(kPacketSize, 0);
		uint32_t writeLen = 0;

		if(!block.data())
			std::cout << "BLOCK ERROR\n";
		if(!encoder)
			std::cout << "ENCODER ERROR\n";

		WirehairResult encodeResult = wirehair_encode(
				encoder,
				blockID,
				block.data(),
				kPacketSize,
				&writeLen);

		std::cout << "LOG -> written " <<  writeLen << " to block." <<std::endl;	

		if(encodeResult != Wirehair_Success)
			std::cout << "encoding failed: " << encodeResult << std::endl;
		else
		{
			std::cout << "LOG -> sending " << writeLen <<std::endl;
			
			sent += socket.send_to(
						boost::asio::buffer(block.data(), block.size()), 
						client_endpoint);

			std::cout << "sent: " << sent << std::endl;
			std::cout << "block: " << blockID++ << std::endl;	
			std::cout << std::endl;
		}
	}

	wirehair_free(encoder);
}
uint32_t UDPserver::create_encoder(WirehairCodec& encoder, std::uint32_t fileSize)
{
	uint32_t kPacketSize = 4096U;

	do
	{
		encoder = wirehair_encoder_create(nullptr, vBuffer.data(), vBuffer.size(), kPacketSize);
	}while(!encoder);

	return kPacketSize;
}
void UDPserver::read_fileToVector(std::string filePath)
{
    std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    std::streamsize fileSize = file.tellg();
    char byte;
	file.unsetf(std::ios::skipws);
    file.seekg(0, std::ios::beg);
    vBuffer.reserve(fileSize); // maybe resize?

    if (file.is_open())
    {             
        std::copy(std::istream_iterator<uint8_t>(file),
        std::istream_iterator<uint8_t>(),
        std::back_inserter(vBuffer));        
    }

    file.close();
}
std::ostream& operator<<(std::ostream& out, std::vector<uint8_t>& vec)
{
    for (int i = 0; i < vec.size(); i++)
        out << vec.at(i);
    return out;
}
