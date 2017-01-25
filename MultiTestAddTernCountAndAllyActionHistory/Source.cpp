//  main.cpp
//  LeanForceLearning
//
//  Created by nakagawakenta on 12/29/16.
//  Copyright © 2016 nakagawakenta. All rights reserved.
//

#include "Header.h"
using namespace std;

random_device seed_gen;
mt19937 engine(seed_gen());
auto dist1 = std::bind(std::uniform_int_distribution<int>(0, mapsize), std::mt19937(static_cast<unsigned int>(time(nullptr))));
auto dist = std::bind(std::uniform_int_distribution<int>(0, 40), std::mt19937(static_cast<unsigned int>(time(nullptr))));

//std::uniform_int_distribution<> dist1(0, mapsize);
std::uniform_int_distribution<> action(0, ACTION);
std::uniform_int_distribution<> makerandom(0, 100);

double p1Qvalue[eqSize][eqSize][pqSize][pqSize][TERNCOUNT][ACTION][ACTION][ACTION] = { 0 };
double p2Qvalue[eqSize][eqSize][pqSize][pqSize][TERNCOUNT][ACTION][ACTION][ACTION] = { 0 };

unsigned int map[mapsize][mapsize] = { 0 };

int outputcount = 20 * pow(2, 0);

//Map�̏�����
void resetmap() {
  for (int y = 0; y < mapsize; y++) {
    for (int x = 0; x < mapsize; x++) {
      map[y][x] = 0;
    }
  }
}

State initState(int x, int y) {
  State tmp;
  tmp.first = x;
  tmp.second = y;
  tmp.pterncount = 0;
  tmp.allyactionhistory.clear();

  for (int i = 0; i <= MAXHISTORY; i++) {
    tmp.allyactionhistory.push_back(4);
  }
  return tmp;
}

//�}�b�v�̕\��
void drawMap() {
  for (int x = 0; x < mapsize; x++) {
    for (int y = 0; y < mapsize; y++) {
      cout << map[x][y] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void setPlayer(State player) {
  map[player.first][player.second] = 1;
}

void setEnemy(State enemy) {
  map[enemy.first][enemy.second] = 3;
}

void resetPlayer(State player) {
  map[player.first][player.second] = 0;
}

//�G�ׂ̗Ƀv���C�������邩�ǂ����𒲂ׂ�.
bool checkNexttoEnemy(State player, State enemy) {
  for (int action = 0; action < 5; action++) {
    State tmp = checkCharacter(player, action);
    if (enemy.first == tmp.first && enemy.second == tmp.second) {
      return true;
    }
  }
  return false;
}

//�G���͂ނ��Ƃ��o���Ă��邩���ׂ�
bool checkSurroundbyPlayer(State player1, State player2, State enemy) {
  bool check1 = checkNexttoEnemy(player1, enemy);
  bool check2 = checkNexttoEnemy(player2, enemy);
  if (check1 == true && check2 == true) {
    return true;
  }
  return false;
}

//�L�����N�^�[�������͂ނ��Ƃ��o���邩�ǂ����̊m�F�p���\�b�h
State checkCharacter(State character, int action) {
  const int dx[] = { 0,1,0,-1,0 };
  const int dy[] = { -1,0,1,0,0 };
  int x = character.first + dx[action];
  int y = character.second + dy[action];

  if (mapsize <= x) {
    x = 0;
  }

  if (mapsize <= y) {
    y = 0;
  }

  if (x == -1) {
    x = mapsize - 1;
  }

  if (y == -1) {
    y = mapsize - 1;
  }

  character.first = x;
  character.second = y;

  return character;
}

//character�̈ړ�,map�ɕ\�����邽�߂̏������݂��s���Ƃ���.
State protCharactor(State player, int action) {
  resetPlayer(player);
  player = moveCharacter(player, action);
  setPlayer(player);
  return player;
}

//�G�̃L�����N�^�[���ړ�������
State protEnemyCharactor(State player, int action) {
  resetPlayer(player);
  player = moveCharacter(player, action);
  setEnemy(player);
  return player;
}

//�L�����N�^�[���ړ�����,�ړ����邱�Ƃ��o�����΂��̈ړ����̍��W���Ԃ�
State moveCharacter(State character, int action) {
  const int dx[] = { 0,1,0,-1,0 };
  const int dy[] = { -1,0,1,0,0 };
  int x = character.first + dx[action];
  int y = character.second + dy[action];

  if (action < 0 || action >= ACTION) {
    return character;
  }

  if (mapsize <= x) {
    x = 0;
  }

  if (mapsize <= y) {
    y = 0;

  }

  if (x == -1) {
    x = mapsize - 1;
  }

  if (y == -1) {
    y = mapsize - 1;
  }

  //�ړ��s�\�ȏꍇ�߂�.
  if (map[x][y] != 0) {
    return character;
  }

  character.first = x;
  character.second = y;
  //cout << character.first << " " << character.second << endl;

  return character;
}

/*********************************************************************
 *
 * Q�w�K�p�̃��\�b�h�R
 *
 **********************************************************************/
//Q�e�[�u���̏�����
void initializeQvalue() {
  //Q�l��0.0�ŏ�����
  for (int i = 0; i < eqSize; i++) {
    for (int j = 0; j < eqSize; j++) {
      for (int pi = 0; pi <pqSize; pi++) {
        for (int pj = 0; pj < pqSize; pj++) {
          for (int t = 0; t< TERNCOUNT; t++) {
            for (int k = 0; k < ACTION; k++) {
              p1Qvalue[i][j][pi][pj][t][k][k][k] = 0;
              p2Qvalue[i][j][pi][pj][t][k][k][k] = 0;
            }
          }
        }
      }
    }
  }
}

//Qテーブルの出力
void outputQvalueTable(int gamecount) {
  string filename1 = IntToString(gamecount) + "p1.csv";
  string filename2 = IntToString(gamecount) + "p2.csv";
  ofstream outputQvaldata1;
  outputQvaldata1.open("Qval\\Qdata" + filename1, std::ios::app);
  ofstream outputQvaldata2;
  outputQvaldata2.open("Qval\\Qdata" + filename2, std::ios::app);

  for (int i = 0; i < eqSize; i++) {
    for (int j = 0; j < eqSize; j++) {
      outputQvaldata1 << "," << p1Qvalue[i][j][e_eysight + 1][e_eysight + 1][2] << "," << ",";
      outputQvaldata2 << "," << p2Qvalue[i][j][e_eysight + 1][e_eysight + 1][2] << "," << ",";
    } outputQvaldata1 << endl; outputQvaldata2 << endl;
    for (int j = 0; j < eqSize; j++) {
      outputQvaldata1 << p1Qvalue[i][j][e_eysight + 1][e_eysight + 1][3] << "," << p1Qvalue[i][j][e_eysight + 1][e_eysight + 1][4] << "," << p1Qvalue[i][j][e_eysight + 1][e_eysight + 1][1] << ",";
      outputQvaldata2 << p2Qvalue[i][j][e_eysight + 1][e_eysight + 1][3] << "," << p2Qvalue[i][j][e_eysight + 1][e_eysight + 1][4] << "," << p2Qvalue[i][j][e_eysight + 1][e_eysight + 1][1] << ",";
    } outputQvaldata1 << endl;
    outputQvaldata2 << endl;
    for (int j = 0; j < eqSize; j++) {
      outputQvaldata1 << "," << p1Qvalue[i][j][e_eysight + 1][e_eysight + 1][0] << "," << ",";
      outputQvaldata2 << "," << p2Qvalue[i][j][e_eysight + 1][e_eysight + 1][0] << "," << ",";
    } outputQvaldata1 << endl;
    outputQvaldata2 << endl;
  }
  outputQvaldata1 << endl; outputQvaldata1 << endl;
  outputQvaldata2 << endl; outputQvaldata2 << endl;
  outputQvaldata1.close();
  outputQvaldata2.close();
}


double getMAXQValue(State afterstate, int nextaction, int playernumber) {
  double maxQ = 0;
  if (playernumber == 1) {
    //int nextaction = getMaxQAction(afterstate, playernumber);                 //after�ł̍ő�Q�l���o���s��
    maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.allyfirst][afterstate.allysecond][afterstate.pterncount][afterstate.allyactionhistory[0]][afterstate.allyactionhistory[1]][nextaction];
  }

  if (playernumber == 2) {
    //int nextaction = getMaxQAction(afterstate, playernumber);                 //after�ł̍ő�Q�l���o���s��
    maxQ = p2Qvalue[afterstate.first][afterstate.second][afterstate.allyfirst][afterstate.allysecond][afterstate.pterncount][afterstate.allyactionhistory[0]][afterstate.allyactionhistory[1]][nextaction];
  }
  return maxQ;
}

//学習フェーズ
int MultiQlearningMethod(State p1, State p2, State enemy, int gamecount)
{
  int episodecount = 0;

  int c = gensui + gamecount;
  double AttenuationAlpha = gensui / (double)c;
  double AAlpha = (double)alpha *AttenuationAlpha;
  std::pair<int, int> p1tmpally = { 0,0 };
  std::pair<int, int> p2tmpally = { 0,0 };

  State p1state = initState(e_eysight, e_eysight);
  State p2state = initState(e_eysight, e_eysight);
  State p1afterstate = initState(e_eysight, e_eysight);
  State p2afterstate = initState(e_eysight, e_eysight);

  while (episodecount < EPISODECOUNT) {
    State p1tmpstate = searchRelationEnemy(p1, enemy);
    p1state.first = p1tmpstate.first;
    p1state.second = p1tmpstate.second;
    p1state.pterncount = p1tmpstate.pterncount;

    State p2tmpstate = searchRelationEnemy(p2, enemy);
    p2state.first = p2tmpstate.first;
    p2state.second = p2tmpstate.second;
    p2state.pterncount = p2tmpstate.pterncount;

    //何ターン見ていないかという情報を反映させる．
    p1.pterncount = p1state.pterncount;
    p2.pterncount = p2state.pterncount;

    p1tmpally = searchRelationAlly(p2, p1);
    p1state.allyfirst = p1tmpally.first;
    p1state.allysecond = p1tmpally.second;

    p2tmpally = searchRelationAlly(p1, p2);
    p2state.allyfirst = p2tmpally.first;
    p2state.allysecond = p2tmpally.second;

    //�����_���ɃL�����N�^�[�𓮂���
    if (flag_checkmovenemy == true) {
      enemy = protEnemyCharactor(enemy, action(engine));
    }
    else {
      enemy = protEnemyCharactor(enemy, 4);
    }

    //プレイヤの行動
    int p1action = chooseAnAction(p1state, 1);
    int p2action = chooseAnAction(p2state, 2);

    //味方の行動させる
    p1 = protCharactor(p1, p1action);
    p2 = protCharactor(p2, p2action);

    //味方の行動履歴を保存させる
    p1state.allyactionhistory = addAllyActionHisotry(p1state.allyactionhistory, p2action);
    p2state.allyactionhistory = addAllyActionHisotry(p2state.allyactionhistory, p1action);

    State p1tmpafterstate = searchRelationEnemy(p1, enemy);
    p1afterstate.first = p1tmpafterstate.first;
    p1afterstate.second = p1tmpafterstate.second;
    p1afterstate.pterncount = p1tmpafterstate.pterncount;

    State p2tmpafterstate = searchRelationEnemy(p2, enemy);
    p2afterstate.first = p2tmpafterstate.first;
    p2afterstate.second = p2tmpafterstate.second;
    p2afterstate.pterncount = p2tmpafterstate.pterncount;

    p1.pterncount = p1afterstate.pterncount;
    p2.pterncount = p2afterstate.pterncount;

    p1tmpally = searchRelationAlly(p2, p1);
    p1afterstate.allyfirst = p1tmpally.first;
    p1afterstate.allysecond = p1tmpally.second;

    p2tmpally = searchRelationAlly(p1, p2);
    p2afterstate.allyfirst = p2tmpally.first;
    p2afterstate.allysecond = p2tmpally.second;

    int p1nextaction = chooseAnAction(p1state, 1);
    int p2nextaction = chooseAnAction(p2state, 2);

    //calcReward
    double p1maxQ = getMAXQValue(p1afterstate, p1nextaction, 1);
    double p2maxQ = getMAXQValue(p2afterstate, p2nextaction, 2);

    if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
      calcFinishReward(p1state, p1action, p1maxQ, AAlpha, 1);
      calcFinishReward(p2state, p2action, p2maxQ, AAlpha, 2);
      break;
    }
    else {
      calcFaildReward(p1state, p1action, p1maxQ, AAlpha, 1);
      calcFaildReward(p2state, p2action, p2maxQ, AAlpha, 2);
    }
    episodecount++;
  }
  return episodecount;
}

//評価
void EvaluationFunction(int evacount) {
  std::string foldaname = ".\\Result\\";
  foldaname.append(IntToString(evacount));
  makeDirectory(foldaname);

  int gamecount = 0;
  int episodecount = 0;
  State evalp1 = initState(dist1(), dist1());
  State evalp2 = initState(dist1(), dist1());
  State evalenemy = initState(dist1(), dist1());

  vector<int> tmpv;

  while (gamecount < EVALUATIONCOUNT) {
    setPlayer(evalp1);
    setPlayer(evalp2);
    setEnemy(evalenemy);

    episodecount = MultiQlearningEvaluationMethod(evalp1, evalp2, evalenemy, gamecount, evacount);

    resetmap();
    do {
      evalp1 = initState(dist1(), dist1());
      evalp2 = initState(dist1(), dist1());
      evalenemy = initState(dist1(), dist1());
    } while (evalp1.first == evalenemy.first && evalp1.second == evalenemy.second);

    tmpv.push_back(episodecount);

    gamecount++;
  }
  outputEvalResult(evacount, tmpv);
}

//結果の出力
void outputEvalResult(int evacount, vector<int> tmpv) {
  ofstream evalresultfile;
  string evalfilename = "Result.txt";
  evalresultfile.open("Result\\" + IntToString(evacount) + "\\" + evalfilename, std::ios::app);

  int i = 0;
  for (auto var : tmpv) {
    evalresultfile << i << "," << var << std::endl;
    i++;
  }
  evalresultfile.close();
}

//評価用
int MultiQlearningEvaluationMethod(State p1, State p2, State enemy, int gamecount, int evacount)
{
  int episodecount = 0;
  vector<outputData> tmpd;
  std::pair<int, int> p1tmpally = { 0,0 };
  std::pair<int, int> p2tmpally = { 0,0 };
  State p1state = initState(e_eysight, e_eysight);
  State p2state = initState(e_eysight, e_eysight);

  while (episodecount < EPISODECOUNT) {
    //視界内での状態の把握
    //敵の位置を自分との相対位置で認識
    State p1tmpstate = searchRelationEnemy(p1, enemy);
    p1state.first = p1tmpstate.first;
    p1state.second = p1tmpstate.second;
    p1state.pterncount = p1tmpstate.pterncount;

    State p2tmpstate = searchRelationEnemy(p2, enemy);
    p2state.first = p2tmpstate.first;
    p2state.second = p2tmpstate.second;
    p2state.pterncount = p2tmpstate.pterncount;

    p1.pterncount = p1state.pterncount;
    p2.pterncount = p2state.pterncount;

    p1tmpally = searchRelationAlly(p2, p1);
    p1state.allyfirst = p1tmpally.first;
    p1state.allysecond = p1tmpally.second;
    p2tmpally = searchRelationAlly(p1, p2);
    p2state.allyfirst = p2tmpally.first;
    p2state.allysecond = p2tmpally.second;

    //Q値に基づく行動の選択
    int p1action = chooseEvalAnAction(p1state, 1);
    int p2action = chooseEvalAnAction(p2state, 2);

    //味方の行動履歴を保存させる
    p1state.allyactionhistory = addAllyActionHisotry(p1state.allyactionhistory, p2action);
    p2state.allyactionhistory = addAllyActionHisotry(p2state.allyactionhistory, p1action);


    //ランダムにキャラクターを動かす
    if (flag_checkmovenemy == true) {
      enemy = protEnemyCharactor(enemy, action(engine));
    }
    else {
      enemy = protEnemyCharactor(enemy, 4);
    }

    //行動の実施
    p1 = protCharactor(p1, p1action);
    p2 = protCharactor(p2, p2action);

    tmpd.push_back({ p1.first,p1.second, p2.first,p2.second,enemy.first,enemy.second });
    episodecount++;

    if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
      break;
    }
  }
  if (evacount == MAXGAME) {
    outputEvaluationMoveData(evacount, gamecount, tmpd);
  }
  tmpd.clear();
  return episodecount;
}

void outputEvaluationMoveData(int evacount, int gamecount, std::vector<outputData> tmpd) {
  ofstream resultfile;
  string filename = IntToString(gamecount) + ".csv";
  resultfile.open("Result\\" + IntToString(evacount) + "\\" + filename, std::ios::app);
  int i = 0;
  for (auto var : tmpd) {
    resultfile << i << "," <<
      var.p1first << "," << var.p1second << "," <<
      var.p2first << "," << var.p2second << "," <<
      var.efirst << "," << var.esecond << endl;
    i++;
  }
  resultfile.close();
}

//トーラス図形における味方の相対位置を獲得する.
std::pair<int, int> searchRelationAlly(State playerpositions, State enemypositons) {
  std::pair<int, int> ally = { 0,0 };
  std::pair<int, int> tmp = { 0,0 };

  ally.first = enemypositons.first - playerpositions.first;
  ally.second = enemypositons.second - playerpositions.second;

  //���@���G���荶
  if (playerpositions.first < enemypositons.first) {
    tmp.first = (enemypositons.first - mapsize) - playerpositions.first;
  }
  else {
    tmp.first = (mapsize - playerpositions.first) + enemypositons.first;
  }

  //���@���G���艺
  if (playerpositions.second < enemypositons.second) {
    tmp.second = (enemypositons.second - mapsize) - playerpositions.second;
  }
  else {
    tmp.second = (mapsize - playerpositions.second) + enemypositons.second;
  }

  //���Βl�ŋ������v�Z����
  if (abs(tmp.first) < abs(ally.first)) {
    ally.first = tmp.first;
  }

  if (abs(tmp.second) < abs(ally.second)) {
    ally.second = tmp.second;
  }

  //�����O�̎��̍��W�ł͑S�ČŒ�.
  if (abs(ally.first) > p_esight) {
    ally.first = p_esight;
    ally.second = p_esight;
    return ally;
  }
  if (abs(ally.second) > p_esight) {
    ally.first = p_esight;
    ally.second = p_esight;
    return ally;
  }
  ally.first = ally.first + p_esight;
  ally.second = ally.second + p_esight;
  return ally;
}

//トーラス図形における敵の相対位置を獲得する.
State searchRelationEnemy(State playerpositions, State allyplayer) {
  State ep = { 0,0,0,0,0 };
  State tmp = { 0,0,0,0,0 };

  ep.pterncount = 0;

  ep.first = allyplayer.first - playerpositions.first;
  ep.second = allyplayer.second - playerpositions.second;
  ep.allyfirst = playerpositions.allyfirst;
  ep.allysecond = playerpositions.allysecond;
  ep.pterncount = playerpositions.pterncount;

  //���@���G���荶
  if (playerpositions.first < allyplayer.first) {
    tmp.first = (allyplayer.first - mapsize) - playerpositions.first;
  }
  else {
    tmp.first = (mapsize - playerpositions.first) + allyplayer.first;
  }

  //���@���G���艺
  if (playerpositions.second < allyplayer.second) {
    tmp.second = (allyplayer.second - mapsize) - playerpositions.second;
  }
  else {
    tmp.second = (mapsize - playerpositions.second) + allyplayer.second;
  }

  //���Βl�ŋ������v�Z����
  if (abs(tmp.first) < abs(ep.first)) {
    ep.first = tmp.first;
  }

  if (abs(tmp.second) < abs(ep.second)) {
    ep.second = tmp.second;
  }

  //�����O�̎��̍��W�ł͑S�ČŒ�.
  if (abs(ep.first) > e_eysight) {
    ep.first = e_eysight;
    ep.second = e_eysight;
    if (ep.pterncount < TERNCOUNT) {
      ep.pterncount++;
      return ep;
    }
    //�����O�̂��ߌ����ĂȂ��J�E���g���ǉ�
    return ep;
  }

  if (abs(ep.second) > e_eysight) {
    ep.first = e_eysight;
    ep.second = e_eysight;
    if (ep.pterncount < TERNCOUNT) {
      ep.pterncount++;
      return ep;
    }
    return ep;
  }


  if (e_eysight == 1) { //視野が1の場合
    ep.first = ep.first;
    ep.second = ep.second;
  }

  ep.first = ep.first + e_eysight;
  ep.second = ep.second + e_eysight;
  ep.pterncount = 0;
  return ep;
}

//現在保持している状態とアクション
std::vector<int> addAllyActionHisotry(std::vector<int> actionhistory, int action) {
  std::vector<int>tmp = actionhistory;
  //cout << "HISTORY" << tmp.size() << endl;
  if (tmp.size() >= MAXHISTORY) {
    tmp.erase(tmp.begin());
  }
  tmp.push_back(action);
  return tmp;
}

//Q�l�ɂ����Č����t���������s�����I������.
//��-�O���[�f�B�@
int chooseAnAction(State playerstate, int playernum) {
  //�s�����I��
  int action;
  //int x = 0,y = 0;
  //const int dx[] = { 0,1,0,-1,0 };
  //const int dy[] = { -1,0,1,0,0 };
  //do {
  int randvalue = makerandom(engine);
  if (randvalue < EPSILON) {
    //�����_���I��
    action = randvalue % 5;
  }
  else {
    //�ʏ��́AQ�l���ő剻�����s�����I��
    action = getMaxQAction(playerstate, playernum);
  }
  //x = playerstate.first + dx[action];
  //y = playerstate.second + dy[action];
  //} while(map[x][y] == 0);//移動できるかどうかチェックする.

  return action;
}

int chooseEvalAnAction(State playerstate, int playernum) {
  int action;
  //�ʏ��́AQ�l���ő剻�����s�����I��
  action = getMaxQAction(playerstate, playernum);
  return action;
}



//����state�ɂ����āA�ő���Q�l�ƂȂ��s�����Ԃ�
int getMaxQAction(State state, int playernum) {
  //int x = 0,y = 0;
  //const int dx[] = { 0,1,0,-1,0 };
  //const int dy[] = { -1,0,1,0,0 };
  int action = 4;
  double maxQ = -1.0;
  //do {
  if (playernum == 1) {
    for (int i = 0; i < ACTION; i++) {
      double q = p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][i];
      //�ő�Q�l�ƂȂ��s�����L��
      if (q > maxQ) {
        action = i;
        maxQ = q;
      }
    }
  }

  if (playernum == 2) {
    for (int i = 0; i < ACTION; i++) {
      double q = p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][i];
      //�ő�Q�l�ƂȂ��s�����L��
      if (q > maxQ) {
        action = i;
        maxQ = q;
      }
    }
  }
  //x = state.first + dx[action];
  //y = state.second + dy[action];
  //}
  //while(map[x][y] == 0); //移動できるかどうかチェックする.
  return action;
}


bool calcSuccessReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum) {
  if (playernum == 1) {
    p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] + AttenuationAlpha* (subrewards + ganna * maxQ);
  }

  if (playernum == 2) {
    p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] + AttenuationAlpha* (subrewards + ganna * maxQ);
  }
  return true;
}

bool calcFinishReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum) {
  if (playernum == 1) {
    p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] = (1 - AttenuationAlpha)* p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] + AttenuationAlpha* (rewards + ganna * maxQ);
  }

  if (playernum == 2) {
    p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] + AttenuationAlpha* (rewards + ganna * maxQ);
  }
  return true;
}

bool calcFaildReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum) {
  if (playernum == 1) {
    p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] + AttenuationAlpha* (faildrewards + ganna * maxQ);
  }

  if (playernum == 2) {
    p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.allyfirst][state.allysecond][state.pterncount][state.allyactionhistory[0]][state.allyactionhistory[1]][action] + AttenuationAlpha* (faildrewards + ganna * maxQ);
  }
  return true;
}


//自分で操作し挙動の確認を行う.
int MultiMoveMethod(State p1, State p2, State enemy, int gamecount) {
  int episodecount = 0;
  int a = 4, a2 = 4;
  //int tmp1 = 4;
  State p1state = { 0,0,0,0,0 };
  State p2state = { 0,0,0,0,0 };

  std::pair<int, int> p1tmpally, p2tmpally;
  while (episodecount < EPISODECOUNT) {
    cin >> a;
    cin >> a2;
    //tmp1 = escapeEnemyAction(enemy, p1,p2);
    enemy = protEnemyCharactor(enemy, 4);
    p1 = protCharactor(p1, a);
    p2 = protCharactor(p2, a2);

    p1state = searchRelationEnemy(p1, enemy);
    cout << "Enemy" << p1state.first << "," << p1state.second << endl;

    p2state = searchRelationEnemy(p2, enemy);
    cout << "Enemy" << p2state.first << "," << p2state.second << endl;
    //何ターン見ていないかという情報を反映させる．
    p1.pterncount = p1state.pterncount;
    p2.pterncount = p2state.pterncount;

    //味方の行動履歴を保存させる
    p1.allyactionhistory = addAllyActionHisotry(p1.allyactionhistory, a2);
    p2.allyactionhistory = addAllyActionHisotry(p2.allyactionhistory, a);

    cout << "HISTORY P1" << p1.allyactionhistory[0] << "," << p1.allyactionhistory[1] << endl;
    cout << "HISTORY P2" << p2.allyactionhistory[0] << "," << p2.allyactionhistory[1] << endl;


    p1tmpally = searchRelationAlly(p2, p1);
    cout << "Ally" << p1tmpally.first << "," << p1tmpally.second << endl;

    p2tmpally = searchRelationAlly(p1, p2);
    cout << "Ally" << p2tmpally.first << "," << p2tmpally.second << endl;


    cout << "TernCount" << p1state.pterncount << "," << p2state.pterncount << endl;


    drawMap();

    if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
      break;
    }
  }
  return episodecount;
}


int main(int argc, char const *argv[]) {
  makeDirectory(".\\Result");
  makeDirectory(".\\Qval");

  ofstream resultfile;
  string filename = "Result.txt";
  resultfile.open("Result\\" + filename, std::ios::app);

  State p1 = initState(dist1(), dist1());
  State p2 = initState(dist1(), dist1());
  State enemy = initState(dist1(), dist1());

  setPlayer(p1);
  setPlayer(p2);
  setEnemy(enemy);

  initializeQvalue();
  //�t�H���_���������΍쐬
  //vector<int> tmpv;
  int gamecount = 0;
  int episodecount = 0;
  //���C�����[�v
  while (gamecount < MAXGAME) {
    episodecount = MultiQlearningMethod(p1, p2, enemy, gamecount);
    //episodecount = MultiMoveMethod(p1,p2,enemy,gamecount);
    resetmap();

    while (p1.first == enemy.first && p1.second == enemy.second) {
      p1 = initState(dist1(), dist1());
      p2 = initState(dist1(), dist1());
      enemy = initState(dist1(), dist1());
    }
    setPlayer(p1);
    setPlayer(p2);
    setEnemy(enemy);

    //tmpv.push_back(episodecount);

    //Evaluation
    if (gamecount == outputcount) {
      outputQvalueTable(gamecount);
      EvaluationFunction(gamecount);
      //桁が一つ上がる度に記録する
      outputcount = outputcount * 2;
    }
    resultfile << gamecount << "," << episodecount << std::endl;
    gamecount++;
  }
  //Qval�̏o��
  //outputAllResult(tmpv);
  outputQvalueTable(gamecount);
  EvaluationFunction(gamecount);
  resultfile.close();

  return 0;
}

//void outputAllResult(vector<int> tmpv){
//  ofstream resultfile;
//  string filename = "Result.txt";
//  resultfile.open("Result\\" + filename, std::ios::app);
//  int gamecount = 0;
//  for(auto var : tmpv) {
//    resultfile << gamecount << "," << var << std::endl;
//    gamecount++;
//  }
//  resultfile.close();
//}
