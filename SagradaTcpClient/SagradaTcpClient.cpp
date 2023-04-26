#include "SagradaTcpClient.hpp"
#define TCP_CLIENT_DEBUG

SagradaTcpClient::SagradaTcpClient(std::string remoteAddr_, int remotePort_)
{
	remoteAddr = remoteAddr_;
	remotePort = remotePort_;
	sendFlag = false;

	inboundMessagesWaiting = 0;
	matchOutcomeHistory.clear();
}

void SagradaTcpClient::startServer()
{
	keepRunning = true;
	while(keepRunning)
	{
		socket.connect(remoteAddr, remotePort);
		connected = true;
		std::cout << "server connected\n";
	
		std::thread sendThread(&SagradaTcpClient::send, this);
		std::thread receiveThread(&SagradaTcpClient::receive, this);
		sendThread.join();
		receiveThread.join();
		std::cout << "server disconnected... retrying\n";
	}

	return;
}

void SagradaTcpClient::killserver()
{
	connected = false;
	keepRunning = false;
	socket.disconnect();
}

void SagradaTcpClient::sendMessage(const char* string)
{
	int length = strlen(string);
	if (length <= 0) 
	{
		sendFlag = false;
		return;
	}
	int i = 0;
	for (; i < 1023 && i < length; ++i) 
	{
		sendBuf[i] = string[i];
	}
	sendBuf[i+1] = '\0';
	sendFlag = true;
	return;
}

void SagradaTcpClient::send()
{
	while (keepRunning && connected) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (sendFlag == true) 
		{
			size_t characters = strlen(sendBuf);
			auto status = socket.send(sendBuf, characters);
			std::cout << "send bytes:" << characters << std::endl;
			#ifdef TCP_CLIENT_DEBUG
			//error check the socket
			if (status == sf::TcpSocket::Done)
			{
				std::cout << "recv OK (SagradaTcpClient::send)\n";
			}
			if (status == sf::TcpSocket::NotReady)
			{
				std::cout << "notready (SagradaTcpClient::send)\n";
			}
			if (status == sf::TcpSocket::Error)
			{
				std::cout << "error recv (SagradaTcpClient::send)\n";
			}
			if (status == sf::TcpSocket::Disconnected)
			{
				connected = false;
				std::cout << "client disconnected (SagradaTcpClient::send)\n";
			}
			#endif // !1
			sendFlag = false;
		}
	}
	return;
}

void SagradaTcpClient::receive()
{
	while (keepRunning && connected)
	{
		char readbuf[1024];
		size_t charsReceived;
		auto status = socket.receive(readbuf, sizeof(readbuf), charsReceived);
		#ifdef TCP_CLIENT_DEBUG
		//error check the socket
		if (status == sf::TcpSocket::Done)
		{
			std::cout << "recv OK (SagradaTcpClient::receive)\n";
		}
		if (status == sf::TcpSocket::NotReady)
		{
			std::cout << "notready (SagradaTcpClient::receive)\n";
		}
		if (status == sf::TcpSocket::Error)
		{
			std::cout << "error recv (SagradaTcpClient::receive)\n";
		}
		if (status == sf::TcpSocket::Disconnected)
		{
			connected = false;
			std::cout << "client disconnected (SagradaTcpClient::receive)\n";
		}
		#endif

		readbufferIntoStream(readbuf, charsReceived, incomingStream, ')', inboundMessagesWaiting);
		readIncomingBuffer();
	}
	return;
}

void SagradaTcpClient::readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream, char packetDelim, int& waitingPackets)
{
	char screener = 'a';
	for (int i = 0; i < sizearr; ++i)
	{
		screener = arr[i];
		if (screener == packetDelim)
		{
			++waitingPackets;
		}
		incomingStream << screener;
	}
	return;
}

void SagradaTcpClient::readIncomingBuffer()
{

	//irrelevant since never expect a message to be sent to the client
	while (inboundMessagesWaiting > 0) 
	{
		WinLossEntry winloss;
		incomingStream.get();
		std::getline(incomingStream, winloss.firstPlace, ',');
		std::getline(incomingStream, winloss.secondPlace, ',');
		std::getline(incomingStream, winloss.thirdPlace, ',');
		std::getline(incomingStream, winloss.fourthPlace, ',');
		incomingStream.get();
		matchOutcomeHistory.push_back(winloss);
		--inboundMessagesWaiting;
	}
	return;
}
