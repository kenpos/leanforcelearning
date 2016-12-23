#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <memory.h>
#include <functional>
#include <ctime>

//���ԋ���
struct State {
	int first;
	int second;
	int locate_enemy_count;
};

struct outputData {
	int first;
	int second;
	int efirst;
	int esecond;
};

const int eyesight = 7;//���E�̍L��
const int qSize = eyesight * 2 + 1;//Q�l�̑傫��
const int mapsize = 15;
const int ACTION = 5;
const bool blindcount = true;//���^�[���G�����Ă��Ȃ����@�Ƃ������Ԑ��𔽉f�����邩�ۂ�
const bool checkmovenemy = false;
const int MAXGAME = 5000000;//��
const int EPISODECOUNT = 500;
const int EVALUATIONCOUNT = 10;
const int EPSILON = 30;

const double gamma = 0.8;
const double alpha = 0.1;
const double faild = 0;
const double rewards = 100;
const double subrewards = 1;

//const std::string foldaname = "ganna" + std::to_string(gamma)
//+ "alpha" + std::to_string(alpha)
//+ "rewards" + std::to_string(rewards)
//+ "eysight" + std::to_string(eyesight)
//+ "mapsize" + std::to_string(mapsize)
//+ "MAXGAME" + std::to_string(MAXGAME)
//+ "EPISODECOUNT" + std::to_string(EPISODECOUNT)
//+ "blindcount" + std::to_string(blindcount)
//+ "checkmovenemy" + std::to_string(checkmovenemy)
//+ "EPSILON" + std::to_string(EPSILON);


//static bool checkExistenceOfFolder(const std::string folder_name);
void resetmap();
void drawMap();
void setPlayer(State player);
void setEnemy(State enemy);
void resetPlayer(State player);
bool checkNexttoEnemy(State player, State enemy);
bool checkSurroundbyPlayer(State player1, State player2, State enemy);
State moveCharacter(State character, int action);
State checkCharacter(State character, int action);
State protCharactor(State player, int action);
void initializeQvalue();
State searchRelationEnemy(State playerpositions, State enemypositons);
int getMaxQAction(State state, int playernum);
int chooseAnAction(State playerstate, int playernum);
//int QlearningMethod(State p1, State p2, State enemy, int gamecount);
int SoloQlearningMethod(State p1, State enemy, int gamecount);
bool calcSoloReward(State state, State afterstate, int action, State player, State enemy, long double AttenuationAlpha);
//bool calcReward(State state, State afterstate, int action, State player, State player2, State enemy, int playernumber, long double AttenuationAlpha);
State initState(int, int);
void outputQvalueTable(int gamecount);
int SoloQlearningEvaluationMethod(State p1, State enemy, int gamecount, int evacount);
void EvaluationFunction(int evacount);
void outputEvaluationMoveData(int evacount, int gamecount, std::vector<outputData> d);
