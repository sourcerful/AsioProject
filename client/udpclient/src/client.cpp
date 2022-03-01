#include "include/client.hpp"

UDPclient::UDPclient(std::uint16_t port) : _port(port)
{
	_vBuffer.resize(4096);
	memset(_vBuffer.data(), 0, _vBuffer.size());
}

void UDPclient::run()
{
    std::string relative_path = "assets/";
    file_info fileInfo;

	//Give fake tasks so context doesn't finish. this is for async use 
    std::thread thread_context = std::thread([&] {_io_context.run(); }); //start the context.
	boost::asio::io_context::work idleWork(_io_context);
    boost::system::error_code error;

//	udp::endpoint server_endpoint(boost::asio::ip::make_address("127.0.0.1"), _port);
	udp::socket socket(_io_context, udp::endpoint(udp::v4(), _port)); //the file descriptor / socket descriptor.
	WirehairCodec decoder;
    udp::endpoint sender;
    
	memset(&fileInfo, sizeof(fileInfo), 0); //reset the struct to 0 (good practice)
		
	std::cout << "Waiting for a file to receive..." << std::endl;

    std::size_t bytes_transferred = socket.receive_from(
        boost::asio::buffer(&fileInfo, sizeof(fileInfo)),
        sender);

    std::cout << "connection from " << sender.address() << std::endl;
    std::cout << "file: " << fileInfo.fileName << std::endl;
    std::cout << "size: " << fileInfo.size << std::endl;

	decoder = wirehair_decoder_create(nullptr, fileInfo.size, 4096U);

	if(!decoder)
	{
		std::cout << "failed to create decoder: " << decoder << std::endl;
		return;
	}

    try
    {	    
		std::string folder(fileInfo.fileName);
		relative_path += folder;	

    	_outFile.open(relative_path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    	saveFile(sender, fileInfo.size, socket, decoder);
    }
    catch(std::exception &e)
    {
    	std::cout << e.what() << std::endl;
    }
   
    if (thread_context.joinable())
        thread_context.join();    

	std::cout << "File downloaded." << std::endl;
	socket.close();
	wirehair_free(decoder);
}

void UDPclient::saveFile(udp::endpoint& sender, std::uint32_t maxFileSize, udp::socket& socket, WirehairCodec& decoder)
{
	static int blockID = 1;

    socket.async_receive_from(boost::asio::buffer(_vBuffer.data(), _vBuffer.size()), sender, 0, 
        [&sender, &socket, maxFileSize, this, &decoder](boost::system::error_code ec, std::size_t length)
        {     
            if (ec) 
			{
                std::cerr << ec.message() << std::endl;
                return;
            }
            try
            {
                std::cout << "LOG -> Got " << length;
				std::cout << " size\n";
				std::cout << "block: " << blockID << std::endl;
				
				if(length == 0)
					saveFile(sender, maxFileSize, socket, decoder); 	
				else
				{	
					WirehairResult decodeResult = wirehair_decode(
								decoder,
								blockID++,
								_vBuffer.data(),
								_vBuffer.size());

					if (decodeResult == Wirehair_Success) 
					{
						std::vector<char> finalFile(maxFileSize, 0);

					 	WirehairResult decodeResult = wirehair_recover(
							decoder,
							finalFile.data(),
							maxFileSize);

						if (decodeResult != Wirehair_Success)
						{
							std::cout << "wirehair_recover failed: " << decodeResult << std::endl;
						}
						else 
						{
							_outFile.write(finalFile.data(), maxFileSize);
                    		_io_context.stop();
							return;
						}
					} 
					else if(decodeResult == Wirehair_NeedMore) 
					{
						std::cout << "Downloading data..." << std::endl;
					}
				}
            }
            catch (std::exception& ex)
            {
                std::cout << ex.what() << std::endl;
				_io_context.stop();
                return;
            }

			std::cout << std::endl;
            
			saveFile(sender, maxFileSize, socket, decoder);            
        }
    );    
}
std::ostream& operator<<(std::ostream& out, std::vector<char>& v)
{
    for (int i = 0; i < v.size(); i++)
        out << " " << v.at(i);
   
    return out;
}
