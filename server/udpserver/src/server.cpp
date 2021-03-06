#include "include/server.hpp"

UDPServer::UDPServer(std::uint16_t port) : _port(port)
{
    udp::endpoint endpoint(boost::asio::ip::udp::v4(), _port); //recieves IPv4 connections.
    this->_reciever = endpoint;
}

void UDPServer::run()
{
    std::string relative_path = "assets/";
    std::thread thread_context = std::thread([&] {_io_context.run(); }); //start the context's work.
    //Give fake tasks so context doesn't finish. this is for async use
    boost::asio::io_context::work idleWork(_io_context);
    udp::socket socket(_io_context, _reciever); //the file descriptor / socket descriptor.
    boost::system::error_code error;
	std::vector<char> temp(4096);

    std::cout << "listening." << std::endl;

    udp::endpoint sender;
    file_info fileInfo;
    memset(&fileInfo, sizeof(fileInfo), 0); //reset the struct to 0 (good practice)
	std::fill(temp.begin(), temp.end(), 0); //fill temp vector with 0 
		
    //socket.recieve_from waits for an incoming packet (UDP at this case), fills the buffer with the recieved data
    //and fills the "sender" endpoint with an information about the sender which is also a pair of IP and port. 

    std::size_t bytes_transferred = socket.receive_from(
        boost::asio::buffer(&fileInfo, sizeof(fileInfo)),
        sender,
        0,
        error);

    std::cout << "connection from " << sender.address() << std::endl;
    std::cout << "file: " << fileInfo.fileName << std::endl;
    std::cout << "size: " << fileInfo.size << std::endl;

    try
    {	    
		_vBuffer.resize(4096);
        //several overloads to recieve_from and send_to.
		//socket.send_to(boost::asio::buffer(vBuffer.data(), bytes_transferred), sender);            
		std::string folder(fileInfo.fileName);
		relative_path += folder;	

    	_outFile.open(relative_path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    	saveFile(sender, fileInfo.size, socket, temp);
    }
    catch(std::exception &e)
    {
    	std::cout << e.what() << std::endl;
    }
   
    if (thread_context.joinable())
        thread_context.join();    
	
    std::cout << "I'm done!" << std::endl;
}
// three / is good
void UDPServer::saveFile(udp::endpoint& sender, std::uint32_t maxFileSize, udp::socket& socket, std::vector<char>& temp)
{
    socket.async_receive_from(boost::asio::buffer(_vBuffer.data(), _vBuffer.size()), sender, 0, 
        [&sender, &socket, maxFileSize, &temp, this](boost::system::error_code ec, std::size_t length)
        {     
            if (ec) 
			{
                std::cerr << ec.message() << std::endl;
                return;
            }
            try
            {
                std::cout << "LOG -> Got " << length;
     	    	if(length > 0)
					std::cout << " size, writing...\n";
				else
					std::cout << " size\n";
				if(length == 0 || temp == _vBuffer)
					saveFile(sender, maxFileSize, socket, temp); 	
				else
				{	
					_outFile.write(_vBuffer.data(), length);
                	temp = _vBuffer;
					std::cout << "REACHED POINTER= " << _outFile.tellp() << std::endl;
                	std::cout << "max file size: " << maxFileSize << std::endl;
                	if (_outFile.tellp() == maxFileSize) 
					{
						std::cout << "Stopping!, max file size=" << maxFileSize << ", ptr=" << _outFile.tellp() << std::endl;
                    	_io_context.stop();
                    	return;
                	}
				}
            }
            catch (std::exception& ex)
            {
                std::cout << ex.what() << std::endl;
                _io_context.stop();
                return;
            }

            saveFile(sender, maxFileSize, socket, temp);            
        }
    );    
}
std::ostream& operator<<(std::ostream& out, std::vector<char>& v)
{
    for (int i = 0; i < v.size(); i++)
        out << " " << v.at(i);
   
    return out;
}
