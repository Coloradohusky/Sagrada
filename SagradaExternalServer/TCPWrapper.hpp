#pragma once
#include "Bundler.hpp"

//this is the data object that will be sent back and forth to server and client
struct Payload
{



};


class TCPWrapper
{
public:
	TCPWrapper();
	~TCPWrapper();
	TCPWrapper(int listenPort_, int sendPort_);

	//getters
	int getListenPort();

	int getSendPort();

	int getClientNum();

	std::string* getClients();

	Payload getSendPayload();

	Payload getRecvPayload();

	//setters

	void setListenPort(int newListen);

	void setSendPort(int newSend);

	//setting of clients done with getclients ptr

	void setSendPayload(Payload newSend);

	void setReceiveBuf(Payload newRecv);


	//general functions

	void send();

	void recv();

private:
	
	int listenPort;
	int sendPort;

	int clientNum;
	std::string clients[4];

	Payload sendBuf;
	Payload recvBuf;

	void cleanUp();
};

