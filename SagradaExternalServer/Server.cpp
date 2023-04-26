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
		/*LeaderBoardEntry stuff;
		stuff.firstPlace = "a";
		stuff.secondPlace = "b";
		stuff.thirdPlace = "c";
		stuff.fourthPlace = "d";
		LeaderBoard.push_back(stuff);
		writeToCsv();*/
		//connect the first client
		sf::TcpListener listen;
		listen.listen(listenPort);
		listen.accept(serverSock);
		connected = true;
 		
		std::cout << "client connected: " << serverSock.getRemoteAddress() << " port: " << listenPort << std::endl;


		//start the threads responsible for sending and receiving for simplicitys sake send will invoke the server update functions apart
		//from just sending
		std::thread sendThread(&Server::send, this);
		std::thread recvThread(&Server::receive, this);
		sendThread.join();
		recvThread.join();
		std::cout << "returning to waiting state\n";
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
			return;
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
			if (len <= 0) 
			{
				sendBuf.sendFlag = false;
				continue;
			}
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
	outStream << "//(pst,p1,p1score,true/false,p2,p2score,true/false,p3,p3score,true/false,p4,p4score,true/false,)";
	int LeaderBoardSiz = LeaderBoard.size();
	for (int i = 0; i < LeaderBoardSiz; ++i)
	{
		LeaderBoardEntry entry = LeaderBoard[i];
		outStream << "\n";

		//player one
		outStream << entry.p1 << ',';
		outStream << entry.p1Score << ',';
		if (entry.p1IsWinner) 
		{
			outStream << "1,";
		}
		else 
		{
			outStream << "0,";
		}
		
		//player two
		outStream << entry.p2 << ',';
		outStream << entry.p2Score << ',';
		if (entry.p2IsWinner)
		{
			outStream << "1,";
		}
		else
		{
			outStream << "0,";
		}

		//player three 
		outStream << entry.p3 << ',';
		outStream << entry.p3Score << ',';
		if (entry.p3IsWinner)
		{
			outStream << "1,";
		}
		else
		{
			outStream << "0,";
		}

		//player four 
		outStream << entry.p4 << ',';
		outStream << entry.p4Score << ',';
		if (entry.p4IsWinner)
		{
			outStream << "1,";
		}
		else
		{
			outStream << "0";
		}
	}

	LeaderBoard.clear();
	outStream.close();
}

void Server::readCsv()
{
	//(pst,p1,p1score,true/false,p2,p2score,true/false,p3,p3score,true/false,p4,p4score,true/false,)
	LeaderBoard.clear();
	std::ifstream inStream;
	inStream.open("winhistory.csv");

	std::string throwaway;
	std::getline(inStream, throwaway);

	while (!inStream.eof()) 
	{
		LeaderBoardEntry entry;
		std::string p1;
		std::string p1Score;
		std::string p1IsWinner;

		std::string p2;
		std::string p2Score;
		std::string p2IsWinner;

		std::string p3;
		std::string p3Score;
		std::string p3IsWinner;

		std::string p4;
		std::string p4Score;
		std::string p4IsWinner;

		int p1IntScore;
		int p2IntScore;
		int p3IntScore;
		int p4IntScore;

		bool p1isWinner;
		bool p2isWinner;
		bool p3isWinner;
		bool p4isWinner;

		std::getline(inStream, p1, ',');
		std::getline(inStream, p1Score, ',');
		std::getline(inStream, p1IsWinner, ',');

		std::getline(inStream, p2, ',');
		std::getline(inStream, p2Score, ',');
		std::getline(inStream, p2IsWinner, ',');

		std::getline(inStream, p3, ',');
		std::getline(inStream, p3Score, ',');
		std::getline(inStream, p3IsWinner, ',');

		std::getline(inStream, p4, ',');
		std::getline(inStream, p4Score, ',');
		std::getline(inStream, p4IsWinner);
	
		p1IntScore = std::stoi(p1Score);
		p2IntScore = std::stoi(p2Score);
		p3IntScore = std::stoi(p3Score);
		p4IntScore = std::stoi(p4Score);

		p1isWinner = std::stoi(p1IsWinner);
		p2isWinner = std::stoi(p2IsWinner);
		p3isWinner = std::stoi(p3IsWinner);
		p4isWinner = std::stoi(p4IsWinner);

		entry.p1 = p1;
		entry.p1Score = p1IntScore;
		entry.p1IsWinner = p1isWinner;

		entry.p2 = p2;
		entry.p2Score = p2IntScore;
		entry.p2IsWinner = p2isWinner;

		entry.p3 = p3;
		entry.p3Score = p3IntScore;
		entry.p3IsWinner = p3isWinner;

		entry.p4 = p4;
		entry.p4Score = p4IntScore;
		entry.p4IsWinner = p4isWinner;

		LeaderBoard.push_back(entry);
	}
	inStream.close();
}

void Server::readIntoOutstream()
{
	for (int i = 0; i < LeaderBoard.size(); ++i)
	{
	//update not expecting two way communications originally to be send when a (upd,) msg received now only storing round info
	}
	sendBuf.sendFlag = true;
}

void Server::prepSendBuf()
{
	//commenting out as server does not need to return messages
	/*if (outStream.eof()) 
	{
		sendBuf.sendFlag = false;
		return;
	}
	int i = 0;
	while(i < 1023 && !outStream.eof()) 
	{
		outStream >> sendBuf.sendMsg[i];
		++i;
	}
	sendBuf.sendMsg[i-1] = '\0';
	sendBuf.sendFlag = true;*/
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
		inStream.get();
		
		std::string msgHeader;
		std::getline(inStream, msgHeader, ',');

		if (msgHeader == "upd")
		{
			readIntoOutstream();
		}
		else if (msgHeader == "pst")
		{
			LeaderBoardEntry ldr;
			std::string p1;
			std::string p1Score;
			std::string p1IsWinner;

			std::string p2;
			std::string p2Score;
			std::string p2IsWinner;

			std::string p3;
			std::string p3Score;
			std::string p3IsWinner;

			std::string p4;
			std::string p4Score;
			std::string p4IsWinner;

			int p1IntScore = 0;
			int p2IntScore = 0;
			int p3IntScore = 0;
			int p4IntScore = 0;


			//read the data in
			std::getline(inStream, p1, ',');
			std::getline(inStream, p1Score, ',');
			std::getline(inStream, p1IsWinner, ',');

			std::getline(inStream, p2, ',');
			std::getline(inStream, p2Score, ',');
			std::getline(inStream, p2IsWinner, ',');

			std::getline(inStream, p3, ',');
			std::getline(inStream, p3Score, ',');
			std::getline(inStream, p3IsWinner, ',');

			std::getline(inStream, p4, ',');
			std::getline(inStream, p4Score, ',');
			std::getline(inStream, p4IsWinner, ',');
			
			p1IntScore = std::stoi(p1Score);
			p2IntScore = std::stoi(p2Score);
			p3IntScore = std::stoi(p3Score);
			p4IntScore = std::stoi(p4Score);


			ldr.p1 = p1;
			ldr.p1Score = p1IntScore;
			if (p1IsWinner == "true" || p1IsWinner == "TRUE" || p1IsWinner == "1")
			{
				ldr.p1IsWinner = true;
			}
			else
			{
				ldr.p1IsWinner = false;
			}

			ldr.p2 = p2;
			ldr.p2Score = p2IntScore;
			if (p2IsWinner == "true" || p2IsWinner == "TRUE" || p2IsWinner == "1")
			{
				ldr.p2IsWinner = true;
			}
			else
			{
				ldr.p2IsWinner = false;
			}

			ldr.p3 = p3;
			ldr.p3Score = p3IntScore;
			if (p3IsWinner == "true" || p3IsWinner == "TRUE" || p3IsWinner == "1")
			{
				ldr.p3IsWinner = true;
			}
			else
			{
				ldr.p3IsWinner = false;
			}

			ldr.p4 = p4;
			ldr.p4Score = p4IntScore;
			if (p4IsWinner == "true" || p4IsWinner == "TRUE" || p4IsWinner == "1")
			{
				ldr.p4IsWinner = true;
			}
			else
			{
				ldr.p4IsWinner = false;
			}
			LeaderBoard.push_back(ldr);
		}
		inStream.get();
	}
	return;
}
