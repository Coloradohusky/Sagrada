#pragma once
//bundler is my personal set of methods
#include "Bundler.hpp"
#include "Includer.hpp"


struct WinLossEntry 
{
	std::string firstPlace;
	std::string secondPlace;
	std::string thirdPlace;
	std::string fourthPlace;

};

class SagradaTcpClient
{
public:
	SagradaTcpClient(std::string remoteAddr_, int remotePort_);


	void startServer();
	void killserver();
	void sendMessage(const char* string);

private:

	void send();

	void receive();

	void readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream, char packetDelim, int& waitingPackets);

	void readIncomingBuffer();

	bool connected;
	bool keepRunning;
	int inboundMessagesWaiting;

	std::stringstream incomingStream;
	std::stringstream outBoundStream;

	//there must be a \n in this or it will be a problem
	bool sendFlag;
	char sendBuf[1024];
	
	std::vector<WinLossEntry> matchOutcomeHistory;

	std::string remoteAddr;
	int remotePort;
	sf::TcpSocket socket;


};

