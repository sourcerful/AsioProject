#include "include/server.hpp"

int main()
{
	std::uint16_t port = 2000;
	const WirehairResult initResult = wirehair_init();

	if(initResult != Wirehair_Success)
	{
		std::cout << "Wirehair initialization failed: ERR " << initResult << std::endl;
		return -1;
	}

	UDPserver server(port);
	server.run();

	return 0;
}
