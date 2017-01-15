//
//  Header.h
//  LeanForceLearning
//
//  Created by nakagawakenta on 12/29/16.
//  Copyright © 2016 nakagawakenta. All rights reserved.
//

#ifndef Header_h
#define Header_h

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
  int pdirection;
  int locate_enemy_count;
};

struct outputData {
  int first;
  int second;
  int efirst;
  int esecond;
};

const int e_eysight = 7;//���E�̍L��
const int qSize = e_eysight * 2 + 1;//Q�l�̑傫��
const int p_esight = 7;
const int e_directsight = 3; //
//const int p_qSize = p_esight * 2 + 1;//Q�l�̑傫��
const int mapsize = 15;
const int wallstate =0b11111111;
const int ACTION = 5;
const int PDIRECTION = 8;
const bool flag_pldirection = true;
const bool flag_blindcount = false;//���^�[���G�����Ă��Ȃ����@�Ƃ������Ԑ��𔽉f�����邩�ۂ�
const std::string flag_movenemy = ""; //"ESCAPE","RANDOM",""
const int MAXGAME = 10000000;//��
const int EPISODECOUNT = 2000;
const int EVALUATIONCOUNT = 10000;
const int EPSILON = 40;

const double ganna = 0.8;
const double alpha = 0.1;
const double faild = 0;
const double rewards = 100;
const double subrewards = 0;

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
int resetmap();
void drawMap();
State initState(int, int);
void setPlayer(State player);
void setEnemy(State enemy);
void setWall(State wall);
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
int chooseEvalAnAction(State playerstate, int playernum);

int MultiQlearningMethod(State p1, State p2, State enemy, int gamecount);
void outputQvalueTable(int gamecount);
int MultiQlearningEvaluationMethod(State p1, State p2, State enemy, int gamecount, int evacount);
void EvaluationFunction(int evacount);

int MultiMoveMethod(State p1,State p2,State enemy, int gamecount);
int escapeEnemyAction(State enemy, State p1,State p2);
State protEnemyCharactor(State player, int action);
int escapeEnemyActionDeside(int tmp1action, int tmp2action);


int getMAXQValue(State afterstate, int playernumber);
bool calcSuccessReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum);
bool calcFinishReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernu);
bool calcFaildReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum);

int searchPlayerDirection(State myposi, State player2);
int searchEnemyDirection(State enemy, State player);

void makeDirectory(std::string path) {
  std::string command = "mkdir ";
  command.append(path);
  system(command.c_str());
}

std::string IntToString(int number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}


#endif /* Header_h */
