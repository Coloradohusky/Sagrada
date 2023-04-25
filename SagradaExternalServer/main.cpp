#include "Bundler.hpp"
#include "sagradaUtils.hpp"
#include "Server.hpp"


int main(void) 
{
	Server svr(46580);
	std::thread serverThread(&Server::start, &svr);
	serverThread.join();
	return 0;
}