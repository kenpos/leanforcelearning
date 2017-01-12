#include "Header.h"

using namespace std;

// �����Z���k�E�c�C�X�^�[�@�ɂ����[���������������A
// �n�[�h�E�F�A�������V�[�h�ɂ��ď�����
random_device seed_gen;
mt19937 engine(seed_gen());
auto dist1 = std::bind(std::uniform_int_distribution<int>(0, mapsize), std::mt19937(static_cast<unsigned int>(time(nullptr))));
auto tworand = std::bind(std::uniform_int_distribution<int>(0, 1), std::mt19937(static_cast<unsigned int>(time(nullptr))));

//std::uniform_int_distribution<> dist1(0, mapsize);
std::uniform_int_distribution<> action(0, ACTION);
std::uniform_int_distribution<> makerandom(0, 100);

double p1Qvalue[qSize][qSize][qSize][ACTION] = { 0 };
double p2Qvalue[qSize][qSize][qSize][ACTION] = { 0 };

int movedata1[mapsize][mapsize] = { 0 };
int movedata2[mapsize][mapsize] = { 0 };

unsigned int map[mapsize][mapsize] = { 0 };

int outputcount = 1000000;

void makeDirectory(std::string path) {
  std::string command = "mkdir ";
  command.append(path);
  system(command.c_str());
}

string IntToString(int number)
{
  stringstream ss;
  ss << number;
  return ss.str();
}

int main() {
  makeDirectory("Result");
  makeDirectory("Evaluation");
  makeDirectory("moveData");
  makeDirectory("Qval");

  State p1 = initState(dist1(), dist1());
  //State p2 =  initState(dist1(engine), (int)dist1(engine));
  State enemy = initState(dist1(), dist1());

  setPlayer(p1);
  //setPlayer(p2);
  setEnemy(enemy);

  initializeQvalue();
  //�t�H���_���������΍쐬
  int gamecount = 0;
  int episodecount = 0;
  //���C�����[�v
  vector<int> tmpv;
  while (gamecount < MAXGAME) {
    cout << "GAMECOUNT"<< gamecount <<endl;
    //episodecount = SoloQlearningMethod(p1, enemy, gamecount);
    episodecount = SoloMoveMethod(p1, enemy, gamecount);
    resetmap();

    while (p1.first == enemy.first && p1.second == enemy.second) {
      p1 = initState(dist1(), dist1());
      enemy = initState(dist1(), dist1());
      //	p2 = { (int)dist1(engine), (int)dist1(engine) };
    }
    setPlayer(p1);
    //setPlayer(p2);
    setEnemy(enemy);

    tmpv.push_back(episodecount);

    if (gamecount == outputcount) {
      outputQvalueTable(gamecount);
      EvaluationFunction(gamecount);
      //	//桁が一つ上がる度に記録する
      outputcount = outputcount * 10;
    }

    gamecount++;
  }
  //Qval�̏o��
  outputQvalueTable(gamecount);
  EvaluationFunction(gamecount);

  ofstream resultfile;
  string movedatafilename = "Result.csv";
  resultfile.open("Result/" + movedatafilename, std::ios::app);
  int i = 0;
  for (auto var : tmpv)
  {
    resultfile << i << "," << var << std::endl;
    i++;
  }
  resultfile.close();

  return 0;

}

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

void EvaluationFunction(int evacount) {
  int gamecount = 0;
  int episodecount = 0;
  State evalp1 = initState(dist1(), dist1());
  State evalenemy = initState(dist1(), dist1());

  std::string foldaname = "Evaluation\\";
  foldaname.append(IntToString(evacount));
  makeDirectory(foldaname);

  ofstream evalresultfile;
  string evalfilename = "Result.txt";
  evalresultfile.open("Evaluation/" + IntToString(evacount) + "/" + evalfilename, std::ios::app);

  while (gamecount < EVALUATIONCOUNT) {
    setPlayer(evalp1);
    setEnemy(evalenemy);

    episodecount = SoloQlearningEvaluationMethod(evalp1, evalenemy, gamecount, evacount);
    resetmap();

    do {
      evalp1 = initState(dist1(), dist1());
      evalenemy = initState(dist1(), dist1());
    } while (evalp1.first == evalenemy.first && evalp1.second == evalenemy.second);

    evalresultfile << gamecount << "," << episodecount << std::endl;
    gamecount++;
  }
  evalresultfile.close();

}

//評価用
int SoloQlearningEvaluationMethod(State p1, State enemy, int gamecount, int evacount)
{
  int episodecount = 0;

  //vector<outputData> tmpd;
  ofstream resultfile;
  string filename = IntToString(gamecount) + ".csv";
  resultfile.open("Evaluation/" + IntToString(evacount) + "/" + filename, std::ios::app);

  while (episodecount < EPISODECOUNT) {
    //視界内での状態の把握
    //敵の位置を自分との相対位置で認識
    State p1state = searchRelationEnemy(p1, enemy);

    //何ターン見ていないかという情報を反映させる．
    p1.locate_enemy_count = p1state.locate_enemy_count;

    //Q値に基づく行動の選択
    int p1action = chooseAnAction(p1state, 1);

    //ランダムにキャラクターを動かす
    if (checkmovenemy == "RANDOM") {
      enemy = protEnemyCharactor(enemy, action(engine));
    }
    else if (checkmovenemy == "ESCAPE") {
      int ea = escapeEnemyAction(enemy, p1);
      enemy = protEnemyCharactor(enemy, ea);
      cout << "escape : " << ea << endl;

    }
    else {
      enemy = protEnemyCharactor(enemy, 4);
    }

    //行動の実施
    p1 = protCharactor(p1, p1action);

    //行動を実施した後の相対位置を認識
    State p1afterstate = searchRelationEnemy(p1, enemy);
    if (blindcount == false) {
      p1state.locate_enemy_count = 0;
      p1.locate_enemy_count = 0;
      p1afterstate.locate_enemy_count = 0;
    }
    //tmpd.push_back({ p1.first,p1.second,enemy.first,enemy.second });
    resultfile << episodecount << "," << p1.first << "," << p1.second << "," << enemy.first << "," << enemy.second << endl;

    episodecount++;
    if (checkNexttoEnemy(p1, enemy) == true) {
      break;
    }
  }
  resultfile.close();
  //		outputEvaluationMoveData(evacount, gamecount, tmpd);
  //		tmpd.clear();
  return episodecount;
}


void outputEvaluationMoveData(int evacount, int gamecount, vector<outputData> d) {

  std::string foldaname;
  foldaname.append(IntToString(evacount));

  ofstream outputmovedata;
  stringstream ss;
  ss << gamecount;
  std::string movedatafilename = ss.str() + ".csv";
  outputmovedata.open("Evaluation/" + foldaname + "/" + movedatafilename, std::ios::app);
  int i = 0;
  for (auto var : d)
  {
    outputmovedata << i << "," << var.first << "," << var.second << "," << var.efirst << "," << var.esecond << std::endl;
    i++;
  }
  outputmovedata.close();
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
      for (int m = 0; m < qSize; m++) {
        for (int k = 0; k < ACTION; k++) {
          p1Qvalue[i][j][m][k] = 0.0;
          p2Qvalue[i][j][m][k] = 0.0;
        }
      }
    }
  }
}

//���̒n�_�ɉ��񓞒B�����̂��񐔂��\������.
//���������Ă��Ƃ���
//void initializeMoveData() {
//	//Q�l��0.0�ŏ�����
//	for (int i = 0; i < mapsize; i++) {
//		for (int j = 0; j < mapsize; j++) {
//				movedata1[i][j] = 0;
//				movedata2[i][j] = 0;
//			}
//		}
//	}


//Q�l�̃e�[�u�����t�@�C���ɏo�͂���
void outputQvalueTable(int gamecount) {
  ofstream outputQvaldata1;
  string filename = IntToString(gamecount) + ".csv";

  outputQvaldata1.open("Qval/Qdata" + filename, std::ios::app);
  //Q�l��0.0�ŏ�����
  for (int m = 0; m < qSize; m++) {
    for (int i = 0; i < qSize; i++) {

      for (int j = 0; j < qSize; j++) {
        outputQvaldata1 << "," << p1Qvalue[i][j][m][2] << "," << ",";
      } outputQvaldata1 << endl;
      for (int j = 0; j < qSize; j++) {
        outputQvaldata1 << p1Qvalue[i][j][m][3] << "," << p1Qvalue[i][j][m][4] << "," << p1Qvalue[i][j][m][1] << ",";
      } outputQvaldata1 << endl;
      for (int j = 0; j < qSize; j++) {
        outputQvaldata1 << "," << p1Qvalue[i][j][m][0] << "," << ",";
      } outputQvaldata1 << endl;
    }
    outputQvaldata1 << endl; outputQvaldata1 << endl;
  }
  outputQvaldata1.close();
}

//void outputDoublePlayQvalueTable() {
//	ofstream outputQvaldata1;
//	ofstream outputQvaldata2;
//	string filename1 = "p1.csv";
//	string filename2 = "p2.csv";
//
//	outputQvaldata1.open(".\\" + foldaname + "\\Qval\\Qdata" + filename1, std::ios::app);
//	outputQvaldata2.open(".\\" + foldaname + "\\Qval\\Qdata" + filename2, std::ios::app);
//
//	const int dx[] = { 0,1,0,-1,0 };
//	const int dy[] = { -1,0,1,0,0 };
//	//Q�l��0.0�ŏ�����
//	for (int i = 0; i < qSize; i++) {
//		for (int j = 0; j < qSize; j++) {
//			outputQvaldata1 << "," << p1Qvalue[i][j][2] << "," <<endl;
//			outputQvaldata1 << p1Qvalue[i][j][3] << "," << p1Qvalue[i][j][4] << "," << p1Qvalue[i][j][1] << endl;
//			outputQvaldata1 << "," << p1Qvalue[i][j][0] << "," <<endl;
//
//			outputQvaldata2 << "," << p1Qvalue[i][j][2] << ",";
//			outputQvaldata2 << p1Qvalue[i][j][3] << "," << p1Qvalue[i][j][4] << "," << p1Qvalue[i][j][1];
//			outputQvaldata2 << "," << p1Qvalue[i][j][0] << ",";
//		} outputQvaldata1 << endl; outputQvaldata2 << endl;
//	}
//
//}


//1�G�s�\�[�h���Č�����.
/*
   (1)�G�[�W�F���g�͊��̏���s���ϑ�����
   (2)�C�ӂ̍s���I�������C�s��a�����s����
   (3)���������V���󂯎���
 */
//int QlearningMethod(State p1, State p2, State enemy,int gamecount) {
//	int episodecount = 0;
//
//	//ofstream outputQvaldata1;
//	//ofstream outputQvaldata2;
//	//ofstream outputmovecount1;
//	//ofstream outputmovecount2;
//	stringstream ss;
//	ss << gamecount;
//	//string filename1 = ss.str() + "p1.csv";
//	//string filename2 = ss.str() + "p2.csv";
//	string filename = ss.str() + ".csv";
//	//
//	//outputQvaldata1.open(".\\" + foldaname + "\\Qval\\data" + filename1 , std::ios::app);
//	//outputQvaldata2.open(".\\" + foldaname + "\\Qval\\data" + filename2 , std::ios::app);
//	//outputmovecount1.open(".\\" + foldaname + "\\movecount\\data" + filename1 , std::ios::app);
//	//outputmovecount2.open(".\\" + foldaname + "\\movecount\\data" + filename2 , std::ios::app);
//
//	int c = 100000 + gamecount;
//	double AAlpha = (double)100000 / c;
//	double AttenuationAlpha = (double)alpha *AAlpha;
//
//	while (episodecount < EPISODECOUNT) {
//		//���E���ł̏��Ԃ̔c��
//		//�G�̈ʒu�������Ƃ̑��Έʒu�ŔF��
//		State p1state = searchRelationEnemy(p1, enemy);
//		State p2state = searchRelationEnemy(p2, enemy);
//
//		//Q�l�Ɋ��Â��s���̑I��
//		int p1action = chooseAnAction(p1state,p1,1);
//		int p2action = chooseAnAction(p2state,p2,2);
//
//		//�s���̎��{
//		p1 = protCharactor(p1, p1action);
//		movedata1[p1.first][p1.second]++;
//		p2 = protCharactor(p2, p2action);
//		movedata2[p2.first][p2.second]++;
//
//		//�s�������{�������̑��Έʒu���F��
//		State p1afterstate = searchRelationEnemy(p1, enemy);
//		State p2afterstate = searchRelationEnemy(p2, enemy);
//
//		//���V�̕t�^
//		calcReward(p1state, p1afterstate, p1action, p1, p2, enemy, 1, AttenuationAlpha);
//		calcReward(p2state, p2afterstate, p2action, p1, p2, enemy, 2, AttenuationAlpha);
//
//		//�t�@�C���o��
//		if (MAXGAME -50 < gamecount) {
//			ofstream outputmovedata;
//			outputmovedata.open(".\\" + foldaname + "\\moveData\\" + filename, std::ios::app);
//			outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," << p2.first << "," << p2.second << "," << enemy.first << "," << enemy.second << std::endl;
//		}
//		//drawMap();
//
//		//�Q�[���̏C������
//		episodecount++;
//		if (checkSurroundbyPlayer(p1, p2, enemy) == true) {//���l�p
//		break;
//		}
//	}
//	//std::cout << "GAME:" <<gamecount << " "<< episodecount << " �I���I" << endl;
//
//	outputDoublePlayQvalueTable();
//
//	//MoveCount�̏o��
//	//for (int i = 0; i < mapsize; i++) {
//	//	for (int j = 0; j < mapsize; j++) {
//	//		outputmovecount1 << movedata1[i][j] << ",";
//	//		outputmovecount2 << movedata2[i][j] << ",";
//	//	}
//	//	outputmovecount1 << endl;outputmovecount2 << endl;
//	//}
//	//outputmovecount1 << endl;outputmovecount2 << endl;
//		//outputmovedata.close();
//
//	return episodecount;
//}


int SoloMoveMethod(State p1, State enemy, int gamecount) {
  int episodecount = 0;
  int a = 0;
  int ea = 4;
  while (episodecount < EPISODECOUNT) {
    cin >> a;
    ea = escapeEnemyAction(enemy, p1);
    enemy = protEnemyCharactor(enemy, ea);
    p1 = protCharactor(p1, a);
    drawMap();

    if (checkNexttoEnemy(p1, enemy) == true) {
      break;
    }
  }
  return episodecount;
}

//自機から逃げるように移動する
int escapeEnemyAction(State enemy, State p1) {
  cout << "escapeEnemyAction" << endl;
  int tmp = searchPlayerDirection(enemy, p1);
  cout << tmp << endl;

  int rand = tworand();
  int eaction = 4;
  switch (tmp) {
  case 2:
    return 3; //up;
    break;
  case 4:
    return 2;//right;
    break;
  case 6:
    return 0;//left
    break;
  case 8:
    return 1;//down
    break;

  case 1:
    if (rand == 0) {
      return 3;  //right or up;
    }
    return 2;
    break;
  case 3:
    if (rand == 0) {
      return 3; //right or up;
    }
    return 0;  //left or up;
    break;
  case 7:
    if (rand == 0) {
      return 1; //right or up;
    }
    return 2;  //right or down
    break;
  case 9:
    if (rand == 0) {
      return 1; //right or up;
    }
    return 0;  //left or down
    break;
  }
  return eaction;
}


int searchPlayerDirection(State myposi, State player2) {
  int direction = 5;

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
    return direction;
  }
  if (abs(pp.second) > p_esight) {
    pp.first = p_esight;
    pp.second = p_esight;
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



int SoloQlearningMethod(State p1, State enemy, int gamecount)
{
  cout <<"1" << endl;
  int episodecount = 0;

  ofstream outputmovedata;
  stringstream ss;
  ss << gamecount;
  string movedatafilename = ss.str() + ".csv";

  int c = 100000 + gamecount;
  double AttenuationAlpha = (double)100000 / (double)c;
  double AAlpha = (double)alpha *AttenuationAlpha;
  cout <<"2" << endl;

  if (MAXGAME - 50 < gamecount) {
    outputmovedata.open("moveData/" + movedatafilename, std::ios::app);
  }

  while (episodecount < EPISODECOUNT) {
    //���E���ł̏��Ԃ̔c��
    //�G�̈ʒu�������Ƃ̑��Έʒu�ŔF��
    State p1state = searchRelationEnemy(p1, enemy);
    p1.locate_enemy_count = p1state.locate_enemy_count;
    cout <<"3" << endl;
    cout <<"X:" <<enemy.first <<"Y"<< enemy.second << endl;

    //Q�l�Ɋ��Â��s���̑I��
    if (checkmovenemy == "RANDOM") {
      enemy = protEnemyCharactor(enemy, action(engine));
    }
    else if (checkmovenemy == "ESCAPE") {
      int ea = escapeEnemyAction(enemy, p1);
      enemy = protEnemyCharactor(enemy, ea);
    }
    else {
      enemy = protEnemyCharactor(enemy, 4);
      cout <<"3b" << endl;
    }
    cout <<"4" << endl;

    //�s���̎��{
    int p1action = chooseAnAction(p1state, 1);
    p1 = protCharactor(p1, p1action);
    //movedata1[p1.first][p1.second]++;
    cout <<"5" << endl;

    //�s�������{�������̑��Έʒu���F��
    State p1afterstate = searchRelationEnemy(p1, enemy);
    if (blindcount == false) {
      p1state.locate_enemy_count = 0;
      p1.locate_enemy_count = 0;
      p1afterstate.locate_enemy_count = 0;
    }
    cout <<"6" << endl;

    //���V�̕t�^
    calcSoloReward(p1state, p1afterstate, p1action, p1, enemy, AAlpha);
    cout <<"7" << endl;

    //���X�g50�Q�[���̃t�@�C�������o��


    if (MAXGAME - 50 < gamecount) {
      outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," << enemy.first << "," << enemy.second << std::endl;
      outputmovedata.close();
    }

    //drawMap();
    //�Q�[���̏C������
    episodecount++;
    if (checkNexttoEnemy(p1, enemy) == true) {
      cout <<"8" << endl;

      break;
    }
  }
  return episodecount;
}

//�G�Ǝ��@�̎������ł̑��΋������Z�o����.
//�����O�̏ꍇ���E�̒����ƂȂ��_�����@�����W�f�[�^�Ƃ��ĕԂ�.
State searchRelationEnemy(State playerpositions, State enemypositons) {
  //std::cout << "searchRelationEnemy" << std::endl;
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
  if (abs(ep.first) > eyesight) {
    ep.first = eyesight;
    ep.second = eyesight;
    //�����O�̂��ߌ����ĂȂ��J�E���g���ǉ�
    if (ep.locate_enemy_count < qSize - 1) {
      ep.locate_enemy_count++;
    }
    return ep;
  }
  if (abs(ep.second) > eyesight) {
    ep.first = eyesight;
    ep.second = eyesight;
    if (ep.locate_enemy_count < qSize - 1) {
      ep.locate_enemy_count++;
    }
    return ep;
  }

  //���Έʒu�Ɏ��E���𑫂����Ƃ�Q�l���i�[�����ʒu�𒲐�����.
  ep.first = ep.first + eyesight;
  ep.second = ep.second + eyesight;

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

//����state�ɂ����āA�ő���Q�l�ƂȂ��s�����Ԃ�
int getMaxQAction(State state, int playernum) {
  double maxQ = -1.0;
  int action = 4;
  for (int i = 0; i < ACTION; i++) {
    double q = p1Qvalue[state.first][state.second][state.locate_enemy_count][i];
    //�ő�Q�l�ƂȂ��s�����L��
    if (q > maxQ) {
      action = i;
      maxQ = q;
    }

    //else if (q == maxQ) {
    //	int rnd = rand100(mt);
    //	int r = rnd % 2;
    //	if (r == 1) {
    //		action = i;
    //		maxQ = q;
    //	}
    //}
  }
  return action;
}


//���V�֐�
bool calcSoloReward(State state, State afterstate, int action, State player, State enemy, long double AttenuationAlpha) {
  double maxQ = 0;
  //���΍��W�ňړ��������Ԃ��擾����
  //std::Statestate =  moveCharacter(player, action);	//a���������̏���
  //std::Stateafter = searchRelationEnemy(player, enemy); //���΍��W�ɕϊ�
  int nextaction = getMaxQAction(afterstate, 1);    //after�ł̍ő�Q�l���o���s��
  maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.locate_enemy_count][nextaction]; //after�ł̍ő�Q�l
  if (checkNexttoEnemy(player, enemy) == true) {
    p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (rewards + ganna * maxQ);
    return true;
  }
  p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (faild + ganna * maxQ);
  return false;
}
