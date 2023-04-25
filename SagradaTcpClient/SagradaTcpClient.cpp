#include "SagradaTcpClient.hpp"

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

std::stringstream* SagradaTcpClient::getOutBoundStream()
{
	return &outBoundStream;
}

void SagradaTcpClient::send()
{
	while (keepRunning && connected) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		prepareSendBuf();
		if (sendFlag == true) 
		{
			size_t characters = strlen(sendBuf);
			auto status = socket.send(sendBuf, characters);
			
			#ifndef TCP_CLIENT_DEBUG
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
		#ifndef TCP_CLIENT_DEBUG
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

void SagradaTcpClient::prepareSendBuf()
{
	for (int i = 0; i < sizeof(sendBuf) && !outBoundStream.eof(); ++i)
	{
		outBoundStream >> sendBuf[i];
	}
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
