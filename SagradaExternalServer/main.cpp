#include "Bundler.hpp"
#include "sagradaUtils.hpp"
#include "Server.hpp"


int main(void) 
{
	Server svr();
	svr.start();

	std::thread server(&Server::start, &svr);


	svr.killServer();
	server.join();
	return 0;
}