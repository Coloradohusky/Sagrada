#pragma once
#include "Bundler.hpp"
#include "Includer.hpp"
#include "Player.hpp"

struct sendData 
{
	bool sendFlag;
	char sendMsg[1024];
};
//data
//(element, element, element, element, )
//gamestate data this will hold onto the important data that the game is doing
struct GameState 
{
	// changes every round
	std::vector<int> playerOrder; // 1, 2, 2, 1, 0 OR 2, 1, 1, 2, 0
	DiceBag diceBag;
	RoundTrack roundTrack;
	int currentTurn = 1;

	// changes every turn
	std::vector<sf::RectangleShape> dicePoolShapes; // set at beginning of the round, then one rectangle gets removed each turn
	std::vector<Player> players = { Player(1), Player(2) };
	DicePool dicePool;
	int currentPlayer = 1;
	int currentPlayerIsDone = 0;
	
	// gets set at the start / end of the game
	int playGame = 0;
	std::vector<PatternCard> selectedPatternCards; // random
	std::vector<PublicObjective> selectedPublicObjectives; // random
	// also the private objectives get set at the start of the game

	
};


class Server
{
public:
	Server(int listenPortOne_, int listenPortTwo_);
	~Server();


	//setters

	//getters
	sendData* getDataOne();

	sendData* getDataTwo();

	//general functions
	int start();

	void killServer();


private:

	void setupGame();

	void updateGameState();

	void receive();

	void send();

	void readbufferIntoStream(char arr[], int sizearr, std::stringstream& stream,char packetDelim, int player);

	void readP1Stream(char delim);

	bool keepRunning;
	bool connected;

	//keeps track of the current turn
	int currentTurn;

	//socket information
	int listenPortOne;
	int listenPortTwo;
	sf::TcpSocket playerOneSock;
	sf::TcpSocket playerTwoSock;


	sendData playerOneData;
	sendData playerTwoData;

	//integers that represent if the stream has a fully formed valid packet to read
	int playerOnePackets;
	int playerTwoPackets;

	//stream data for the players tcp connections
	std::stringstream playerOneStream;
	std::stringstream playerTwoStream;
	
	

	GameState state;

};

