#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <memory.h>

template < typename T > std::string to_string( const T& n )
{
		std::ostringstream stm ;
		stm << n ;
		return stm.str() ;
}

//���ԋ���
struct State {
	int first;
	int second;
	int locate_enemy_count;
};

const int eyesight = 4;//���E�̍L��
const int qSize = eyesight * 2 + 1;//Q�l�̑傫��
const int mapsize = 9;
const int ACTION = 5;
const bool blindcount = true;//���^�[���G�����Ă��Ȃ����@�Ƃ������Ԑ��𔽉f�����邩�ۂ�
const bool checkmovenemy = false;
const int MAXGAME = 200000000;//��
const int EPISODECOUNT = 500; //1�G�s�\�[�h�ӂ��ǂ̒��x

const int EVALUATIONCOUNT = 10000;//���@�����̕]���G�s�\�[�h��



const int EPSILON = 40;

const double ganna = 0.8;
const double alpha = 0.1;
const double faild = 0;
const double rewards = 100;
const double subrewards = 0;

const std::string foldaname = "ganna" + to_string(gamma)
+ "alpha" +   to_string(alpha)
+ "rewards" + to_string(rewards)
+ "eysight" + to_string(eyesight)
+ "mapsize" + to_string(mapsize)
+ "MAXGAME" + to_string(MAXGAME)
+ "EPISODECOUNT" + to_string(EPISODECOUNT)
+ "blindcount" + to_string(blindcount)
+ "checkmovenemy" + to_string(checkmovenemy)
+ "EPSILON" + to_string(EPSILON);


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
int QlearningMethod(State p1, State p2, State enemy, int gamecount);
int QlearningEvaluationMethod(State p1, State p2, State enemy, int gamecount, int evacount);
State initState(int, int);
void outputQvalueTable(int gamecount);
void outputEvaluationQvalueTable(int evacount);
int chooseAnEvaluationAction(State playerstate, int playernum);
void EvaluationFunction(int evacount);
bool calcReward(State p1state, State p1afterstate, State p2state, State p2afterstate, int action, int action2, State p1, State p2, State enemy, long double AttenuationAlpha);

//�t�@�C���o�͊֌W
//static bool checkExistenceOfFolder(const std::string folder_name) {
//	if (_mkdir(folder_name.c_str()) == 0) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
