//////////////////////////////////////////////////////////////////////////////////
//name: Justin Van Der Sluys													//
//date last edited: 4/26/2023													//
//file: SagradaTcpClient.hpp													//
//description:Sagrada Tcp client basically handles sending of data to a server 	//
//that keeps track of the match history											//
//																				//
//																				//
//////////////////////////////////////////////////////////////////////////////////

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


//networking is slowly driving me insane
class SagradaTcpClient
{
public:
	SagradaTcpClient(std::string remoteAddr_, int remotePort_);

	//description:tries to establish a connection with the server when it does it starts the send and receive threads
	//params:none
	//return:none
	//pre-conditions:none
	//post conditions:none
	void startServer();

	//description:kills the server disconnects the server basically used to kill the threads and rejoin
	//params:none
	//return:none
	//pre-conditions:none
	//post conditions:none
	void killserver();

	//description: pushes a string to the send buffer and sets the sendflag to go
	//params: const char* string
	//return:nothing
	//pre-conditions: string should have a /0 in it
	//post conditions: string should be sent by the buffer
	void sendMessage(const char* string);

private:

	//description:threaded function that continuously checks to see if the send flag is set and then sends the buffer
	//params:none
	//return:none
	//pre-conditions: socket should be ready to go and connected
	//post conditions: none
	void send();

	//description: threaded fucntion that continously tries to receive a message from the server then it reads it and deals with it
	//params:void
	//return:void
	//pre-conditions:socket should be ready to go and connected
	//post conditions:void
	void receive();

	//description:reads a array into a stream and then increments the waiting packets according to how many delimiters were found
	//params:char arr[], int sizearr, std::stringstream& stream, char packetDelim, int& waitingPackets
	//return:void
	//pre-conditions:none
	//post conditions:none
	void readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream, char packetDelim, int& waitingPackets);

	//description: reads the incoming stream buffer and handles the data
	//params:none
	//return:none
	//pre-conditions:none
	//post conditions:none
	void readIncomingBuffer();

	bool connected;
	bool keepRunning;


	int inboundMessagesWaiting;

	std::stringstream incomingStream;
	std::stringstream outBoundStream;

	//there must be a \0 in this or it will be a problem
	bool sendFlag;
	char sendBuf[1024];
	
	std::vector<WinLossEntry> matchOutcomeHistory;

	std::string remoteAddr;
	int remotePort;
	sf::TcpSocket socket;


};

