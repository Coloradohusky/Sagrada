#include "Bundler.hpp"
#include "Includer.hpp"
#include "SagradaTcpClient.hpp"

int main(void) 
{
	SagradaTcpClient client("25.49.77.220", 46580);
	std::thread svr(&SagradaTcpClient::startServer, &client);
	while (true) 
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		*client.getOutBoundStream() << "(pst,ClientPlayer1,696969,1,clientplayer2,42069,1,clientplayer3,420420,1,clientplayer4,51224,0,)";
	}
	svr.join();
	return 0;
}