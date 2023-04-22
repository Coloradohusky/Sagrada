#include "Server.hpp"

Server::Server(int listenPortOne_, int listenPortTwo_)
{
}

Server::~Server()
{
}

int Server::start()
{
	setupGame();
	currentTurn = 1;
	while (keepRunning)
	{

		//connect the first client
		sf::TcpListener listenOne;
		listenOne.listen(listenPortOne);
		listenOne.accept(playerOneSock);
		std::cout << "client One connected: " << playerOneSock.getRemoteAddress() << " port: " << listenPortOne << std::endl;

		//connect the second client
		sf::TcpListener listenTwo;
		listenTwo.listen(listenPortTwo);
		listenTwo.accept(playerTwoSock);
		std::cout << "client two connected: " << playerTwoSock.getRemoteAddress() << " port: " << listenPortOne + 1 << std::endl;


		//start the threads responsible for sending and receiving for simplicitys sake send will invoke the server update functions apart
		//from just sending
		std::thread sendThread(&Server::send, this);
		std::thread recvThread(&Server::receive, this);
		sendThread.join();
		recvThread.join();
	}
	return;
}

void Server::killServer()
{
	keepRunning = false;
	connected = false;
}
//(data, data, data, data, data,)
void Server::setupGame()
{
	//needs to setup the game 
	state;
	// can I use Sagrada classes?
	for (int i = 0; i < 8; i++) {
		int randomNumber = patternDist(mt);
		while (windowFrames.find(std::to_string(randomNumber)) == windowFrames.not_found()) {
			randomNumber = patternDist(mt);
		}
		for (int j = 0; j < 2; j++) {
			selectedPatternCards.insert(selectedPatternCards.end(), PatternCard());
			selectedPatternCards.back().setName(windowFrames.get_child(std::to_string(randomNumber)).front().first);
			selectedPatternCards.back().setTokens(std::stoi(windowFrames.get_child(std::to_string(randomNumber)).front().second.get<std::string>("Tokens")));
			for (int k = 0; k < 4; k++) {
				boost::property_tree::ptree temp = windowFrames.get_child(std::to_string(randomNumber)).front().second.get_child("Board").begin()->second;
				int subSubIndex = 0;
				fillFrameArray(temp, &selectedPatternCards, (i * 2) + j, k, subSubIndex);
				windowFrames.get_child(std::to_string(randomNumber)).front().second.get_child("Board").pop_front();
			}
			windowFrames.get_child(std::to_string(randomNumber)).pop_front();
		}
		windowFrames.erase(std::to_string(randomNumber));
	}

	for (int i = 0; i < 3; i++) {
		int randomNumber = objectiveDist(mt);
		randomNumber = 3;
		while (publicObjectives.find(std::to_string(randomNumber)) == publicObjectives.not_found()) {
			randomNumber = patternDist(mt);
		}
		selectedPublicObjectives.insert(selectedPublicObjectives.end(), PublicObjective());
		selectedPublicObjectives.back().setName(publicObjectives.get_child(std::to_string(randomNumber)).get<std::string>("Name"));
		selectedPublicObjectives.back().setDescription(publicObjectives.get_child(std::to_string(randomNumber)).get<std::string>("Description"));
		selectedPublicObjectives.back().setPoints(publicObjectives.get_child(std::to_string(randomNumber)).get<std::string>("Points"));
		publicObjectives.erase(std::to_string(randomNumber));
	}
}


void Server::receive()
{
	char receiveBuffer[1024];
	size_t bufSize = 0;
	while (connected && keepRunning) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (currentTurn == 1) 
		{
			auto status = playerOneSock.receive(receiveBuffer, sizeof(receiveBuffer), bufSize);
			
			//debugging stuff define the NETWORKDEBUG to get verbose info
			#ifdef NETWORKDEBUG
			if (messageStat == sf::TcpSocket::Done)
			{
				std::cout << "recv OK Send p1\n";
			}
			if (messageStat == sf::TcpSocket::NotReady)
			{
				std::cout << "notready Send p1\n";
			}
			if (messageStat == sf::TcpSocket::Error)
			{
				std::cout << "error recv Send p1\n";
			}
			if (messageStat == sf::TcpSocket::Disconnected)
			{
				std::cout << "client disconnected Send p1\n";
			}
			#endif // DEBUG

			if (status == sf::TcpSocket::Error || status == sf::TcpSocket::Disconnected)
			{
				connected = false;
			}

		}
		//receive for the second player
		if (currentTurn == 2) 
		{
			auto status = playerTwoSock.receive(receiveBuffer, sizeof(receiveBuffer), bufSize);
			#ifdef NETWORKDEBUG
			if (messageStat == sf::TcpSocket::Done)
			{
				std::cout << "recv OK Send p2\n";
			}
			if (messageStat == sf::TcpSocket::NotReady)
			{
				std::cout << "notready Send p2\n";
			}
			if (messageStat == sf::TcpSocket::Error)
			{
				std::cout << "error recv Send p2\n";
			}
			if (messageStat == sf::TcpSocket::Disconnected)
			{
				std::cout << "client disconnected Send p2\n";
			}
			#endif // DEBUG

			if (status == sf::TcpSocket::Error || status == sf::TcpSocket::Disconnected)
			{
				connected = false;
			}
		

		}
		else 
		{
			throw "current turn gave something invalid (Server::receive)";
		}
	}
	return;
}

void Server::send()
{
	while (connected && keepRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (currentTurn == 1)
		{
			//if the sendflag it set it needs to be sent
			if (playerOneData.sendFlag == true)
			{
				size_t msg = strlen(playerOneData.sendMsg);
				auto messageStat = playerOneSock.send(playerOneData.sendMsg, msg);

				#ifdef NETWORKDEBUG
				if (messageStat == sf::TcpSocket::Done)
				{
					std::cout << "recv OK Send p1\n";
				}
				if (messageStat == sf::TcpSocket::NotReady)
				{
					std::cout << "notready Send p1\n";
				}
				if (messageStat == sf::TcpSocket::Error)
				{
					std::cout << "error recv Send p1\n";
				}
				if (messageStat == sf::TcpSocket::Disconnected)
				{
					std::cout << "client disconnected Send p1\n";
					connected = false;
				}
				#endif // DEBUG
				playerOneData.sendFlag = false;
			}
		}
		else if (currentTurn == 2) 
		{
			if (playerTwoData.sendFlag == true) 
			{
				size_t msg = strlen(playerOneData.sendMsg);
				auto messageStat = playerOneSock.send(playerOneData.sendMsg, msg);

				#ifdef NETWORKDEBUG
				if (messageStat == sf::TcpSocket::Done)
				{
					std::cout << "recv OK Send p2\n";
				}
				if (messageStat == sf::TcpSocket::NotReady)
				{
					std::cout << "notready Send p2\n";
				}
				if (messageStat == sf::TcpSocket::Error)
				{
					std::cout << "error recv Send p2\n";
				}
				if (messageStat == sf::TcpSocket::Disconnected)
				{
					std::cout << "client disconnected Send p2\n";
					connected = false;
				}
				#endif // DEBUG
				playerTwoData.sendFlag = false;
			}
		}
		else 
		{
			throw "current turn gave something invalid (Server::send)";
		}
	}
	return;
}

void Server::readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream, char packetDelim, int player)
{
	for (int i = 0; i < sizearr; ++i) 
	{
		char scanner = arr[i];
		if (scanner == packetDelim)
		{
			if (player == 1) 
			{
				++playerOnePackets;
			}
			else 
			{
				++playerTwoPackets;
			}
			stream << scanner;
		}
	}


	return;
}
