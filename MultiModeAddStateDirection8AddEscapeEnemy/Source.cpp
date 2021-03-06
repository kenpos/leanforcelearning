//
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
auto tworand = std::bind(std::uniform_int_distribution<int>(0, 1), std::mt19937(static_cast<unsigned int>(time(nullptr))));

//std::uniform_int_distribution<> dist1(0, mapsize);
std::uniform_int_distribution<> action(0, ACTION);
std::uniform_int_distribution<> makerandom(0, 100);

// 敵X　敵Y　味方居る方向　何ターン敵を見ていないか　行動
double p1Qvalue[qSize][qSize][PDIRECTION][qSize][ACTION] = { 0 };
double p2Qvalue[qSize][qSize][PDIRECTION][qSize][ACTION] = { 0 };

unsigned int map[mapsize][mapsize] = { 0 };

int outputcount = pow(2,0);

int main() {
  makeDirectory(".\\Result");
  makeDirectory(".\\Evaluation");
  makeDirectory(".\\moveData");
  makeDirectory(".\\Qval");

  resetmap();
  State p1;
  State p2;
  State enemy;

  //マップ上の壁や敵味方が被らないように調整.
  do {
    p1 = initState(dist1(), dist1());
    p2 = initState(dist1(), dist1());
    enemy = initState(dist1(), dist1());
  } while((map[p1.first][p1.second] != 2 )&&
          (map[p2.first][p2.second] != 2 )&&
          (enemy.first == p1.first && enemy.second == p1.second) &&
          (enemy.first == p2.first && enemy.second == p2.second)&&
          (p1.first == p2.first && p1.second == p2.second) );
  //State wall = initState(dist1(), dist1());

  setPlayer(p1);
  setPlayer(p2);
  setEnemy(enemy);
  //setWall(wall);

  initializeQvalue();
  //�t�H���_���������΍쐬

  ofstream resultfile;
  string filename = "Result.txt";
  resultfile.open("Result/" + filename, std::ios::app);

  int gamecount = 0;
  int episodecount = 0;
  //���C�����[�v
  while (gamecount < MAXGAME) {
    episodecount = MultiQlearningMethod(p1, p2, enemy, gamecount);
    //episodecount = MultiMoveMethod(p1, p2, enemy,gamecount);

    resetmap();
//被っていないようにマップを作成する.
    while ( (map[p1.first][p1.second] != 2 )&&
            (map[p2.first][p2.second] != 2 )&&
            (enemy.first == p1.first && enemy.second == p1.second) &&
            (enemy.first == p2.first && enemy.second == p2.second)&&
            (p1.first == p2.first && p1.second == p2.second) ) {
      p1 = initState(dist1(), dist1());
      p2 = initState(dist1(), dist1());
      enemy = initState(dist1(), dist1());
    }
    setPlayer(p1);
    setPlayer(p2);
    setEnemy(enemy);

    resultfile << gamecount << "," << episodecount << std::endl;

    //Evaluation
    if (gamecount == outputcount) {
      outputQvalueTable(gamecount);
      EvaluationFunction(gamecount);
      //桁が一つ上がる度に記録する
      outputcount = outputcount * 2;
    }
    gamecount++;
  }
  //Qval�̏o��
  outputQvalueTable(gamecount);
  EvaluationFunction(gamecount);
  resultfile.close();


  return 0;
}

//自分で操作し挙動の確認を行う.
int MultiMoveMethod(State p1,State p2,State enemy, int gamecount) {
  int episodecount = 0;
  int a = 0;
  int tmp1 = 4;

  while (episodecount < EPISODECOUNT) {
    cin >> a;
    //cin >> a2;

    tmp1 = escapeEnemyAction(enemy, p1,p2);

    enemy = protEnemyCharactor(enemy, tmp1);
    p1 = protCharactor(p1, a);
    p2 = protCharactor(p2, 4);

    drawMap();

    if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
      break;
    }
  }
  return episodecount;
}

//Map�̏�����
//void resetmap() {
///////
////////////////  for (int y = 0; y < mapsize; y++) {
/////////////////////    for (int x = 0; x < mapsize; x++) {
////////////////      map[y][x] = 0;
///////    }
//  }
//}

int resetmap(){
  cout<<"ResetMap" << endl;

  //ファイルの読み込み
  ifstream ifs("map.csv");
  if(!ifs) {
    cout<<"入力エラー" << endl;
    cout << "恐らくファイルが見つからないものと思います" << endl;
    return -1;
  }

  //csvファイルを1行ずつ読み込む
  string str;
  int row = 0;//行
  int col = 0;//列
  while(getline(ifs,str)) {
    string token;
    istringstream stream(str);
    //1行のうち、文字列とコンマを分割する
    while(getline(stream,token,',')) {
      istringstream stream(token);
      //すべて文字列として読み込まれるため
      //数値は変換が必要
      //int temp=stof(token);       //stof(string str) : stringをfloatに変換
      map[row][col] = stof(token);
      cout<<"row" <<row <<":" << "col" << col<< endl;
      col++;
    }
    col = 0;
    row++;
  }
  return 0;
}

State initState(int x, int y) {
  State tmp;
  tmp.first = x;
  tmp.second = y;
  tmp.locate_enemy_count = 0;
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

void setWall(State wall) {
  map[wall.first][wall.second] = 2;
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
  for (int i = 0; i < qSize; i++) {
    for (int j = 0; j < qSize; j++) {
      for (int n = 0; n < PDIRECTION; n++) {
        for (int m = 0; m < qSize; m++) {
          for (int k = 0; k < ACTION; k++) {
            p1Qvalue[i][j][n][m][k] = 0.0;
            p2Qvalue[i][j][n][m][k] = 0.0;
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
  outputQvaldata1.open("Qval/Qdata" + filename1, std::ios::app);
  ofstream outputQvaldata2;
  outputQvaldata2.open("Qval/Qdata" + filename2, std::ios::app);

  for (int m = 0; m < qSize; m++) {
    for (int n = 0; n < PDIRECTION; n++) {
      for (int i = 0; i < qSize; i++) {
        for (int j = 0; j < qSize; j++) {
          outputQvaldata1 << "," << p1Qvalue[i][j][n][m][2] << "," << ",";
          outputQvaldata2 << "," << p2Qvalue[i][j][n][m][2] << "," << ",";
        } outputQvaldata1 << endl; outputQvaldata2 << endl;
        for (int j = 0; j < qSize; j++) {
          outputQvaldata1 << p1Qvalue[i][j][m][3] << "," << p1Qvalue[i][j][m][4] << "," << p1Qvalue[i][j][m][1] << ",";
          outputQvaldata2 << p2Qvalue[i][j][m][3] << "," << p2Qvalue[i][j][m][4] << "," << p2Qvalue[i][j][m][1] << ",";
        } outputQvaldata1 << endl;
        outputQvaldata2 << endl;
        for (int j = 0; j < qSize; j++) {
          outputQvaldata1 << "," << p1Qvalue[i][j][m][0] << "," << ",";
          outputQvaldata2 << "," << p2Qvalue[i][j][m][0] << "," << ",";
        } outputQvaldata1 << endl;
        outputQvaldata2 << endl;
      }
      outputQvaldata1 << endl; outputQvaldata1 << endl;
      outputQvaldata2 << endl; outputQvaldata2 << endl;
    } outputQvaldata1 << endl; outputQvaldata1 << endl;
    outputQvaldata2 << endl; outputQvaldata2 << endl;
  }
  outputQvaldata1.close();
  outputQvaldata2.close();
}


int getMAXQValue(State afterstate, int playernumber) {
  double maxQ = 0;
  if (playernumber == 1) {
    int nextaction = getMaxQAction(afterstate, playernumber);                 //after�ł̍ő�Q�l���o���s��
    maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.pdirection][afterstate.locate_enemy_count][nextaction];
  }

  if (playernumber == 2) {
    int nextaction = getMaxQAction(afterstate, playernumber);                 //after�ł̍ő�Q�l���o���s��
    maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.pdirection][afterstate.locate_enemy_count][nextaction];
  }
  return maxQ;
}

void EvaluationFunction(int evacount) {
  int gamecount = 0;
  int episodecount = 0;
  State evalp1 = initState(dist1(), dist1());
  State evalp2 = initState(dist1(), dist1());
  State evalenemy = initState(dist1(), dist1());

  std::string foldaname = ".\\Result\\";
  foldaname.append(IntToString(evacount));
  makeDirectory(foldaname);

  ofstream evalresultfile;
  string evalfilename = "Result.txt";
  evalresultfile.open("Result\\" + IntToString(evacount) + "\\" + evalfilename, std::ios::app);

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

    evalresultfile << gamecount << "," << episodecount << std::endl;
    gamecount++;
  }
  evalresultfile.close();
}

//評価用
int MultiQlearningEvaluationMethod(State p1, State p2, State enemy, int gamecount, int evacount)
{
  int episodecount = 0;

  //vector<outputData> tmpd;
  ofstream resultfile;
  string filename = IntToString(gamecount) + ".csv";
  resultfile.open("Evaluation\\" + IntToString(evacount) + "\\" + filename, std::ios::app);

  while (episodecount < EPISODECOUNT) {
    //視界内での状態の把握
    //敵の位置を自分との相対位置で認識
    State p1state = searchRelationEnemy(p1, enemy);
    p1state.pdirection = searchPlayerDirection(p1, p2);
    State p2state = searchRelationEnemy(p2, enemy);
    p2state.pdirection = searchPlayerDirection(p2, p1);

    //何ターン見ていないかという情報を反映させる．
    p1.locate_enemy_count = p1state.locate_enemy_count;
    p2.locate_enemy_count = p2state.locate_enemy_count;

    //Q値に基づく行動の選択
    int p1action = chooseEvalAnAction(p1state, 1);
    int p2action = chooseEvalAnAction(p2state, 2);

    //ランダムにキャラクターを動かす
    if (flag_movenemy == "RANDOM") {
      enemy = protEnemyCharactor(enemy, action(engine));
    }else if(flag_movenemy == "ESCAPE") {
      int tmp1 = escapeEnemyAction(enemy, p1,p2);
      enemy = protEnemyCharactor(enemy, tmp1);
    }else{
      enemy = protEnemyCharactor(enemy, 4);
    }

    //行動の実施
    p1 = protCharactor(p1, p1action);
    p2 = protCharactor(p2, p2action);

    if (flag_blindcount == false) {
      p1state.locate_enemy_count = 0;
      p1.locate_enemy_count = 0;
      p2state.locate_enemy_count = 0;
      p2.locate_enemy_count = 0;
    }

    if (flag_pldirection == false) {
      p1state.pdirection = 4;
      p1.pdirection = 4;
      p2state.pdirection = 4;
      p2.pdirection = 4;
    }


    //tmpd.push_back({ p1.first,p1.second,enemy.first,enemy.second });
    resultfile << episodecount << "," <<
      p1.first << "," << p1.second << "," <<
      p2.first << "," << p2.second << "," <<
      enemy.first << "," << enemy.second << endl;

    episodecount++;
    if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
      break;
    }
  }
  resultfile.close();
  //outputEvaluationMoveData(evacount, gamecount, tmpd);
  //tmpd.clear();
  return episodecount;
}

int MultiQlearningMethod(State p1, State p2, State enemy, int gamecount)
{
  int episodecount = 0;

  ofstream outputmovedata;
  stringstream ss;
  ss << gamecount;
  string movedatafilename = ss.str() + ".csv";

  int c = 100000 + gamecount;
  double AttenuationAlpha = (double)100000 / (double)c;
  double AAlpha = (double)alpha *AttenuationAlpha;

  if (MAXGAME - 50 < gamecount) {
    outputmovedata.open("moveData/" + movedatafilename, std::ios::app);
  }

  while (episodecount < EPISODECOUNT) {
    State p1state = searchRelationEnemy(p1, enemy);
    p1state.pdirection = searchPlayerDirection(p1, p2);

    State p2state = searchRelationEnemy(p2, enemy);
    p2state.pdirection = searchPlayerDirection(p2, p1);

    //Q�l�Ɋ��Â��s���̑I��
    int p1action = chooseAnAction(p1state, 1);
    int p2action = chooseAnAction(p2state, 2);

    //�����_���ɃL�����N�^�[�𓮂���
    if (flag_movenemy == "RANDOM") {
      enemy = protEnemyCharactor(enemy, action(engine));
    }else if(flag_movenemy == "ESCAPE") {
      int tmp1 = escapeEnemyAction(enemy, p1,p2);
      enemy = protEnemyCharactor(enemy, tmp1);
    }else{
      enemy = protEnemyCharactor(enemy, 4);
    }

    //�s���̎��{
    p1 = protCharactor(p1, p1action);
    p2 = protCharactor(p2, p2action);

    State p1afterstate = searchRelationEnemy(p1, enemy);
    p1afterstate.pdirection = searchPlayerDirection(p1, p2);

    State p2afterstate = searchRelationEnemy(p2, enemy);
    p2afterstate.pdirection = searchPlayerDirection(p2, p1);


    if (flag_blindcount == false) {
      p1state.locate_enemy_count = 0;
      p1.locate_enemy_count = 0;
      p1afterstate.locate_enemy_count = 0;
      p2state.locate_enemy_count = 0;
      p2.locate_enemy_count = 0;
      p2afterstate.locate_enemy_count = 0;
    }

    if (flag_pldirection == false) {
      p1state.pdirection = 4;
      p1.pdirection = 4;
      p1afterstate.pdirection = 4;
      p2state.pdirection = 4;
      p2.pdirection = 4;
      p2afterstate.pdirection = 4;
    }


    if (MAXGAME - 50 < gamecount) {
      outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," <<p2.first <<"," << p2.second<<","<< enemy.first << "," << enemy.second << std::endl;
      outputmovedata.close();
    }

    //calcReward
    double p1maxQ = getMAXQValue(p1afterstate, 1);
    double p2maxQ = getMAXQValue(p2afterstate, 2);

    if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
      calcFinishReward(p1state, p1action, p1maxQ, AAlpha, 1);
      calcFinishReward(p2state, p2action, p2maxQ, AAlpha, 2);
      break;
    }

    if (checkNexttoEnemy(p1, enemy) == true) {
      calcSuccessReward(p1state, p1action, p1maxQ, AAlpha, 1);
    }
    else {
      calcFaildReward(p1state, p1action, p1maxQ, AAlpha, 1);
    }

    if (checkNexttoEnemy(p2, enemy) == true) {
      calcSuccessReward(p2state, p2action, p2maxQ, AAlpha, 2);
    }
    else {
      calcFaildReward(p2state, p2action, p2maxQ, AAlpha, 2);
    }

    episodecount++;
  }
  return episodecount;
}

//味方がいる方向を検出する.
int searchPlayerDirection(State myposi, State player2) {
  int direction = 0;

  State pp;
  State tmp = { 0,0 };
  pp.first = player2.first - myposi.first;
  pp.second = player2.second - myposi.second;

  if (myposi.first < player2.first) {
    tmp.first = (player2.first - mapsize) - myposi.first;
  }
  else {
    tmp.first = (mapsize - myposi.first) + player2.first;
  }

  //���@���G���艺
  if (myposi.second < player2.second) {
    tmp.second = (player2.second - mapsize) - myposi.second;
  }
  else {
    tmp.second = (mapsize - myposi.second) + player2.second;
  }

  //���Βl�ŋ������v�Z����
  if (abs(tmp.first) < abs(pp.first)) {
    pp.first = tmp.first;
  }

  if (abs(tmp.second) < abs(pp.second)) {
    pp.second = tmp.second;
  }

  //視界外にいた時.
  if (abs(pp.first) > p_esight) {
    pp.first = p_esight;
    pp.second = p_esight;
    return 4;
  }
  if (abs(pp.second) > p_esight) {
    pp.first = p_esight;
    pp.second = p_esight;
    return 4;
  }

  //四方向を返すところ
  //左右
  if(pp.first == 0) {
    if(pp.second < 0) {
      return 3;
    }
    return 5;
  }
  //上下
  if(pp.second == 0) {
    if(pp.first < 0) {
      return 1;
    }
    return 7;
  }

  //斜め方向の処理
  //左側
  if (pp.first < 0) {
    if (pp.second < 0) {
      return 0;
    }
    return 6;
  }
  //右側
  else {
    if (pp.second < 0) {
      return 2;
    }
    return 8;
  }

  return direction;
}

//敵ががいる方向を検出する.主に,敵の回避行動に利用される.
int searchEnemyDirection(State myposi, State enemy) {
  int direction = 5;

  State pp;
  State tmp = { 0,0 };
  pp.first = enemy.first - myposi.first;
  pp.second = enemy.second - myposi.second;

  if (myposi.first < enemy.first) {
    tmp.first = (enemy.first - mapsize) - myposi.first;
  }
  else {
    tmp.first = (mapsize - myposi.first) + enemy.first;
  }

  //���@���G���艺
  if (myposi.second < enemy.second) {
    tmp.second = (enemy.second - mapsize) - myposi.second;
  }
  else {
    tmp.second = (mapsize - myposi.second) + enemy.second;
  }

  //���Βl�ŋ������v�Z����
  if (abs(tmp.first) < abs(pp.first)) {
    pp.first = tmp.first;
  }

  if (abs(tmp.second) < abs(pp.second)) {
    pp.second = tmp.second;
  }

  //視界外にいた時.
  if (abs(pp.first) > e_directsight) {
    return direction;
  }
  if (abs(pp.second) > e_directsight) {
    return direction;
  }

  //四方向を返すところ
  //左右
  if (pp.first == 0) {
    if (pp.second < 0) {
      return 4;
    }
    return 6;
  }
  //上下
  if (pp.second == 0) {
    if (pp.first < 0) {
      return 8;
    }
    return 2;
  }

  //斜め方向の処理
  //左側
  if (pp.first < 0) {
    if (pp.second < 0) {
      return 7;
    }
    return 9;
  }
  //右側
  else {
    if (pp.second < 0) {
      return 1;
    }
    return 3;
  }

  return direction;
}

int switchAction(int num){
  int rand = tworand();
  switch (num) {
//4方向から攻める
  case 2:
    return 3;                 //up;
    break;
  case 4:
    return 2;                 //right;
    break;
  case 6:
    return 0;                 //left
    break;
  case 8:
    return 1;                 //down
    break;
//斜め方向から攻める場合
  case 1:
    if (rand == 0) {
      return 3;                       //right or up;
    }
    return 2;
    break;
  case 3:
    if (rand == 0) {
      return 3;                       //right or up;
    }
    return 0;                 //left or up;
    break;
  case 7:
    if (rand == 0) {
      return 1;                       //right or up;
    }
    return 2;                 //right or down
    break;
  case 9:
    if (rand == 0) {
      return 1;                       //right or up;
    }
    return 0;                 //left or down
    break;
  default:
    break;
  }
  return 5;
}

//自機から逃げるように移動する
int escapeEnemyAction(State enemy, State p1,State p2) {
  cout << "escapeEnemyAction" << endl;
  int tmp = searchEnemyDirection(enemy, p1);
  int tmp2 = searchEnemyDirection(enemy, p2);

  int eaction = 4;

  //両方のプレイヤが見えている場合
  if(tmp != 5 && tmp2 != 5 ) {
    //同じ方向から攻める場合.
    if(tmp == tmp2) {
      eaction = switchAction(tmp);
      return eaction;
    }else{            //違う方向から攻める場合
      if(tmp < 4 && tmp2 < 4) { //下方向から追いかける場合
        eaction = 3; //up
        return eaction;
      }
      if(tmp > 6 && tmp2 > 6 ) {//上方向から
        eaction = 1; //down
        return eaction;
      }
      if(tmp % 3 == 0  && tmp2 % 3 ==0 ) {//右方向から
        eaction = 0; //left
        return eaction;
      }

      if(tmp % 3 == 1  && tmp2 % 3 == 1 ) {//左方向から
        eaction = 2; //right
        return eaction;
      }

      if((tmp < 4 && tmp2 > 6) ||(tmp > 6 && tmp2 < 4)) { //プレイヤが上下から取り囲んでいた時
        int rand = tworand();
        if (rand == 0) {
          eaction = 0;    //right or left;
        }
        eaction = 2;
        return eaction;
      }

      if((tmp % 3 == 1  && tmp2 % 3 == 0 ) || (tmp % 3 == 0  && tmp2 % 3 == 1 )) { //プレイヤが左右から取り囲んでいた時
        int rand = tworand();
        if (rand == 0) {
          eaction = 1;    //up or down;
        }
        eaction = 3;
        return eaction;
      }


    }
  }else{              //どっちか片方のプレイヤが見えている
    //p1のみ見えている場合
    if(tmp2 == 5) {
      eaction = switchAction(tmp);
      return eaction;
    }
    //p2のみ見えている場合
    if(tmp == 5) {
      eaction =  switchAction(tmp2);
      return eaction;
    }
  }
  return eaction;
}


State searchRelationEnemy(State playerpositions, State enemypositons) {
  State ep;
  State tmp = { 0,0 };

  ep.first = enemypositons.first - playerpositions.first;
  ep.second = enemypositons.second - playerpositions.second;
  ep.locate_enemy_count = playerpositions.locate_enemy_count;

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
    //�����O�̂��ߌ����ĂȂ��J�E���g���ǉ�
    if (ep.locate_enemy_count < qSize - 1) {
      ep.locate_enemy_count++;
    }
    return ep;
  }
  if (abs(ep.second) > e_eysight) {
    ep.first = e_eysight;
    ep.second = e_eysight;
    if (ep.locate_enemy_count < qSize - 1) {
      ep.locate_enemy_count++;
    }
    return ep;
  }

  //���Έʒu�Ɏ��E���𑫂����Ƃ�Q�l���i�[�����ʒu�𒲐�����.
  ep.first = ep.first + e_eysight;
  ep.second = ep.second + e_eysight;

  ep.locate_enemy_count = 0;

  return ep;
}


//Q�l�ɂ����Č����t���������s�����I������.
//��-�O���[�f�B�@
int chooseAnAction(State playerstate, int playernum) {
  int randvalue = makerandom(engine);
  //�s�����I��
  int action;
  if (randvalue < EPSILON) {
    //�����_���I��
    action = randvalue % 5;

  }
  else {
    //�ʏ��́AQ�l���ő剻�����s�����I��
    action = getMaxQAction(playerstate, playernum);
  }
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
  int action = 4;
  double maxQ = -1.0;

  if (playernum == 1) {
    for (int i = 0; i < ACTION; i++) {
      double q = p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][i];
      //�ő�Q�l�ƂȂ��s�����L��
      if (q > maxQ) {
        action = i;
        maxQ = q;
      }
    }
  }

  if (playernum == 2) {
    for (int i = 0; i < ACTION; i++) {
      double q = p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][i];
      //�ő�Q�l�ƂȂ��s�����L��
      if (q > maxQ) {
        action = i;
        maxQ = q;
      }
    }
  }
  return action;
}

bool calcSuccessReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum) {
  if (playernum == 1) {
    p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] + AttenuationAlpha* (subrewards + ganna * maxQ);
  }

  if (playernum == 2) {
    p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] + AttenuationAlpha* (subrewards + ganna * maxQ);
  }
  return true;
}

bool calcFinishReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum) {
  if (playernum == 1) {
    p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] = (1 - AttenuationAlpha)* p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] + AttenuationAlpha* (rewards + ganna * maxQ);
  }

  if (playernum == 2) {
    p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] + AttenuationAlpha* (rewards + ganna * maxQ);
  }
  return true;
}

bool calcFaildReward(State state, int action, double maxQ, long double AttenuationAlpha, int playernum) {
  if (playernum == 1) {
    p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] + AttenuationAlpha* (faild + ganna * maxQ);
  }

  if (playernum == 2) {
    p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.pdirection][state.locate_enemy_count][action] + AttenuationAlpha* (faild + ganna * maxQ);
  }
  return true;
}
