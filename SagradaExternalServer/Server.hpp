#pragma once
#include "Bundler.hpp"
#include "TCPWrapper.hpp"


class Server
{
public:
	Server();
	~Server();

	
	int start();



private:
	int currentTurn;




};

