#include "Server.hpp"

Server::Server(int listenPortOne_)
{
	keepRunning = true;
	connected = true;
	listenPort = listenPortOne_;
	waitingPackets = 0;

	sendBuf.sendFlag = false;
	LeaderBoard.clear();
}

Server::~Server()
{
}

void Server::start()
{
	
	while (keepRunning)
	{
		readCsv();
		//connect the first client
		sf::TcpListener listen;
		listen.listen(listenPort);
		listen.accept(serverSock);
		std::cout << "client One connected: " << serverSock.getRemoteAddress() << " port: " << listenPort << std::endl;


		//start the threads responsible for sending and receiving for simplicitys sake send will invoke the server update functions apart
		//from just sending
		std::thread sendThread(&Server::send, this);
		std::thread recvThread(&Server::receive, this);
		sendThread.join();
		recvThread.join();
		writeToCsv();
	}
	return;
}




void Server::receive()
{
	char receiveBuffer[1024];
	size_t bufSize = 0;
	while (connected && keepRunning) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		auto messageStat = serverSock.receive(receiveBuffer, sizeof(receiveBuffer), bufSize);
		if (messageStat == sf::TcpSocket::Done)
		{
			std::cout << "recv OK\n";
		}
		if (messageStat == sf::TcpSocket::NotReady)
		{
			std::cout << "notready\n";
		}
		if (messageStat == sf::TcpSocket::Error)
		{
			std::cout << "error recv\n";
		}
		if (messageStat == sf::TcpSocket::Disconnected)
		{
			std::cout << "client disconnected\n";
			connected = false;
		}

		readbufferIntoStream(receiveBuffer, bufSize, inStream, ')', waitingPackets);
		readMessages();
	}
	return;
}

void Server::send()
{
	while (connected && keepRunning)
	{
		prepSendBuf();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (sendBuf.sendFlag) 
		{
			size_t len = strlen(sendBuf.sendMsg);
			auto messageStat = serverSock.send(sendBuf.sendMsg, len);
			if (messageStat == sf::TcpSocket::Done)
			{
				std::cout << "recv OK Send\n";
			}
			if (messageStat == sf::TcpSocket::NotReady)
			{
				std::cout << "notready Send\n";
			}
			if (messageStat == sf::TcpSocket::Error)
			{
				std::cout << "error recv Send\n";
			}
			if (messageStat == sf::TcpSocket::Disconnected)
			{
				std::cout << "client disconnected Send";
				connected = false;
			}
			sendBuf.sendFlag = false;
		}
	}
	return;
}

void Server::readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream, char packetDelim, int& waitingPackets)
{
	for (int i = 0; i < sizearr; ++i) 
	{
		char scanner = arr[i];
		if (scanner == packetDelim)
		{
			++waitingPackets;
		}
		stream << scanner;
	}
	return;
}

void Server::writeToCsv()
{
	std::ofstream outStream;
	outStream.open("winhistory.csv");
	outStream << "Firstplace,secondPlace,thirdplace,fourthplace\n";

	for (int i = LeaderBoard.size()-1; i >= 0; ++i)
	{
		LeaderBoardEntry entry = LeaderBoard[i];
		outStream << entry.firstPlace << ',';
		outStream << entry.secondPlace << ',';
		outStream << entry.thirdPlace << ',';
		outStream << entry.fourthPlace << '\n';
	}

	LeaderBoard.clear();
	outStream.close();
}

void Server::readCsv()
{
	LeaderBoard.clear();
	std::ifstream inStream;
	inStream.open("winhistory.csv");

	std::string throwaway;
	std::getline(inStream, throwaway);

	while (!inStream.eof()) 
	{
		LeaderBoardEntry entry;
		std::getline(inStream, entry.firstPlace, ',');
		std::getline(inStream, entry.secondPlace, ',');
		std::getline(inStream, entry.thirdPlace, ',');
		std::getline(inStream, entry.fourthPlace);
		
		LeaderBoard.push_back(entry);
	}
	inStream.close();
}

void Server::readIntoOutstream()
{
	for (int i = 0; i < LeaderBoard.size(); ++i)
	{
		outStream << '(' << LeaderBoard[i].firstPlace << ',';
		outStream << '(' << LeaderBoard[i].secondPlace << ',';
		outStream << '(' << LeaderBoard[i].thirdPlace << ',';
		outStream << '(' << LeaderBoard[i].fourthPlace << ',';
		outStream << ')';
	}
	sendBuf.sendFlag = true;
}

void Server::prepSendBuf()
{
	if (outStream.eof()) 
	{
		sendBuf.sendFlag = false;
		return;
	}
	for (int i = 0; i < 1023 && !outStream.eof(); ++i) 
	{
		outStream << sendBuf.sendMsg[i];
	}
	outStream << '\n';
	return;
}

void Server::readMessages()
{

	//messages
	//(upd,) request for scoreinfo
	//(pst,p1,p2,p3,p4,) post score 
	for (; waitingPackets > 0; --waitingPackets) 
	{
		char throwaway = 'a';
		inStream >> throwaway;
		
		std::string msgHeader;
		std::getline(inStream, msgHeader, ',');

		if (msgHeader == "upd")
		{
			readIntoOutstream();
		}
		else if (msgHeader == "pst")
		{
			LeaderBoardEntry ldr;
			std::getline(inStream, ldr.firstPlace, ',');
			std::getline(inStream, ldr.secondPlace, ',');
			std::getline(inStream, ldr.thirdPlace, ',');
			std::getline(inStream, ldr.fourthPlace, ',');
			LeaderBoard.push_back(ldr);
		}
		inStream >> throwaway;
	}
	return;
}
