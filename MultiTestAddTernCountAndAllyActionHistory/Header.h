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
        int allyfirst;
        int allysecond;
        int pterncount;
        std::vector<int> allyactionhistory;
};

struct outputData {
        int p1first;
        int p1second;
        int p2first;
        int p2second;
        int efirst;
        int esecond;
};

const int e_eysight = 3; //敵の周囲何マスを格納するのか確認する
const int eqSize = e_eysight * 2 + 1; //敵の相対位置を格納する
const int p_esight = 7; //自分の周囲何マスを見ているのか判定する
const int pqSize = p_esight * 2 + 1; //味方の相対位置を格納する
const int TERNCOUNT = 21; //何ターン敵を見ていないかを数える
const int MAXHISTORY = 2; //味方の過去の行動を保持
const int ACTION = 5;
const int HISTORY = pow(ACTION, MAXHISTORY); //
const int HISTPARM = 1;//何個前のデータから閲覧するのか

const int mapsize = 15;
const int MAXSIGHT = 8;
const bool flag_checkmovenemy = false;

const int MAXGAME = 100000000;//��
const int EPISODECOUNT = 2000;
const int EVALUATIONCOUNT = 5000;
const int EPSILON = 10; //0.02

const double gensui = 1000000;
const double ganna = 0.8;
const double alpha = 0.1;
const double faildrewards = 0;
const double rewards = 100;
const double subrewards = 0;

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

void resetmap();
void drawMap();
State initState(int, int);
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
std::vector<int> addAllyActionHisotry(std::vector<int> actionhistory, int action);

int getMaxQAction(State state, int playernum);
int chooseAnAction(State playerstate, int playernum);
int chooseEvalAnAction(State playerstate, int playernum);

int MultiQlearningMethod(State p1, State p2, State enemy, int gamecount);
void outputQvalueTable(int gamecount);
int MultiQlearningEvaluationMethod(State p1, State p2, State enemy, int gamecount, int evacount);
void EvaluationFunction(int evacount);
void outputEvaluationMoveData(int evacount, int gamecount, std::vector<outputData> tmpd);
void outputEvalResult(int evacount, std::vector<int> tmpv);

std::pair<int, int> searchRelationAlly(State playerpositions, State enemypositons);

double getMAXQValue(State afterstate, int playernumber);
bool calcSuccessReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum);
bool calcFinishReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum);
bool calcFaildReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum);


#endif /* Header_h */
