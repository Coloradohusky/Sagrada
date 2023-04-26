#pragma once
#include "Bundler.hpp"
#include "Includer.hpp"


struct sendData 
{
	bool sendFlag;
	char sendMsg[1024];
};


//(pst,p1,p1score,true/false,p2,p2score,true/false,p3,p3score,true/false,p4,p4score,true/false,)
struct LeaderBoardEntry 
{
	//first place
	std::string p1;
	int p1Score;
	bool p1IsWinner;

	//second place
	std::string p2;
	int p2Score;
	bool p2IsWinner;

	//third place
	std::string	p3;
	int p3Score;
	bool p3IsWinner;

	//fourth place
	std::string p4;
	int p4Score;
	bool p4IsWinner;
};





class Server
{
public:
	Server(int listenPort);
	~Server();

	std::stringstream* getOutStream();

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

