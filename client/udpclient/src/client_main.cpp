#include "include/client.hpp"

int main()
{
    std::uint16_t port = 2000;
	const WirehairResult initResult = wirehair_init(); //initialize wirehair

	if(initResult != Wirehair_Success)
	{
		std::cout << "Wirehair initialization failed: " << initResult << std::endl;
		return -1;
	}

    UDPclient client(port);
    client.run();

	return 0;
}
