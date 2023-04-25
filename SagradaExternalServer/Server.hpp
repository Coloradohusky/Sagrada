#pragma once
#include "Bundler.hpp"
#include "Includer.hpp"


struct sendData 
{
	bool sendFlag;
	char sendMsg[1024];
};

struct LeaderBoardEntry 
{
	std::string firstPlace;
	std::string secondPlace;
	std::string	thirdPlace;
	std::string fourthPlace;
};





class Server
{
public:
	Server(int listenPort);
	~Server();

	
	void start();

private:
	void receive();

	void send();

	void readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream,char packetDelim, int& waitingPackets);

	void writeToCsv();

	void readCsv();

	void readIntoOutstream();

	void prepSendBuf();

	void readMessages();

	bool keepRunning;
	bool connected;

	std::vector<LeaderBoardEntry> LeaderBoard;

	sendData sendBuf;

	//socket information
	int listenPort;
	sf::TcpSocket serverSock;

	int waitingPackets;
	std::stringstream inStream;
	std::stringstream outStream;
};

