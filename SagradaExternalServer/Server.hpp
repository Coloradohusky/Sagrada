////////////////////////////////////////////////////////////////////////////////////////////
//name: Justin Van Der Sluys															  //
//last edited: 4/26/2023																  //
//file: Server.hpp																		  //
//description: Server class that handles the data from the sagrada						  //
//game. its essentially a csv storage server as it can only get LeaderBoardEntrys		  //
//and store them into the csv file.														  //
////////////////////////////////////////////////////////////////////////////////////////////
//help from this video series https://www.youtube.com/playlist?list=PLk6CEY9XxSIAeK-EAh3hB4fgNvYkYmghp
#pragma once
#include "Bundler.hpp"
#include "Includer.hpp"


struct sendData 
{
	//setting the sendFlag tells the send thread its ready to go and it will send the message immediately this doesnt matter much since I was told to omit sending on the server
	bool sendFlag;
	char sendMsg[1024];
};

//expected packets
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

	//description: waits for the server to connect then starts the send and receive threads ends the threads when the server disconnects and re sets up and waits for more connections
	//special considerations: Server needs to be end to end routable with a open port of 46580 to work I used Hamachi to establish a pseudo lan if things are really not working 
	//use wireshark and packet sender to isolate a problem
	//params:none
	//return:void
	//pre-conditions:none
	//post conditions:none
	void start();

private:

	//description:runs in its own thread basically waits to receive data from the socket when and if it does it reads it into a incomingdata buffer and then calls
	//read messages to handle the data
	//params:none
	//return:none
	//pre-conditions:the socket should be ok and connected for this to work
	//post conditions:none
	void receive();

	//description:runs in its own thread checks to see if the user has filled the sendmessage buffer by looking at the true or false sendflag
	//params:none
	//return:none
	//pre-conditions: if the buffer that is filled does not have a \0 somewhere in bounds it will crash it uses strlen
	//post conditions:none
	void send();

	//description:general function that reads a char array of a particular size into a stream it then looks for a delimiter and if it is encountered it increments the waiting packets ticker
	//params:char arr[], int sizearr, std::stringstream& stream,char packetDelim, int& waitingPackets
	//return: none
	//pre-conditions:sizearr should be the valid size and the stream must not be in a error state
	//post conditions:none
	void readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream,char packetDelim, int& waitingPackets);

	//description:writes the scoreboard data to a csv file
	//params:none
	//return:none
	//pre-conditions:none
	//post conditions:none
	void writeToCsv();

	//description:reads the scoreboard csv into the leaderboard array
	//params:none
	//return:none
	//pre-conditions:none
	//post conditions:csv data should then added to the leaderboard
	void readCsv();

	//description:DO NOT USE not thread safe
	//params:none
	//return:none
	//pre-conditions:none
	//post conditions:should read segments of the stringstream into the sendbuf and set it to send data
	void readIntoOutstream();

	//description: DO NOT USE server does not send messages only receives
	//params: none
	//return: none
	//pre-conditions: none
	//post conditions: sendarray should be populated and set to send
	void prepSendBuf();

	//description:reads the incoming instream untill there are no more fully formed packets inside of it.
	//params:none
	//return:none
	//pre-conditions:should have packets to read
	//post conditions:if any packets came in it should have added more scoreboard entries
	void readMessages();

	bool keepRunning;
	bool connected;

	std::vector<LeaderBoardEntry> LeaderBoard;

	//send data
	sendData sendBuf;

	//socket information
	int listenPort;
	sf::TcpSocket serverSock;

	//how many complete packets are waiting in inStream
	int waitingPackets;

	//this only gets touched by one thread at a time so its fine to use
	std::stringstream inStream;

	//realized this should be using a mutex and locking access but hindsight is 20/20 for the sake of not bricking the project I will stick to char arrays
	std::stringstream outStream;
};

