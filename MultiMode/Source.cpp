#include "Header.h"

using namespace std;

// �n�[�h�E�F�A�������V�[�h�ɂ��ď�����
random_device seed_gen;
mt19937 engine(seed_gen());
auto dist1 = std::bind(std::uniform_int_distribution<int>(0, mapsize), std::mt19937(static_cast<unsigned int>(time(nullptr))));
//std::uniform_int_distribution<> dist1(0, mapsize);
std::uniform_int_distribution<> action(0, ACTION);
std::uniform_int_distribution<> makerandom(0, 100);

double p1Qvalue[qSize][qSize][qSize][ACTION] = { 0 };
double p2Qvalue[qSize][qSize][qSize][ACTION] = { 0 };

unsigned int map[mapsize][mapsize] = { 0 };

int outputcount = 10;

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
	State p2 =  initState(dist1(), dist1());
	State enemy = initState(dist1(), dist1());

	setPlayer(p1);
	setPlayer(p2);
	setEnemy(enemy);

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
		resetmap();

		while (p1.first == enemy.first && p1.second == enemy.second) {
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
			//	//桁が一つ上がる度に記録する
			outputcount = outputcount * 10;
		}

		gamecount++;
	}
	//Qval�̏o��
	outputQvalueTable(gamecount);
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
	State evalp2 = initState(dist1(), dist1());
	State evalenemy = initState(dist1(), dist1());

	std::string foldaname = "Evaluation\\";
	foldaname.append(IntToString(evacount));
	makeDirectory(foldaname);

	ofstream evalresultfile;
	string evalfilename = "Result.txt";
	evalresultfile.open("Evaluation/" + IntToString(evacount) + "/" + evalfilename, std::ios::app);

	while (gamecount < EVALUATIONCOUNT) {
		setPlayer(evalp1);
		setPlayer(evalp2);
		setEnemy(evalenemy);

		episodecount = MultiQlearningEvaluationMethod(evalp1,evalp2, evalenemy, gamecount, evacount);
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
int MultiQlearningEvaluationMethod(State p1,State p2, State enemy, int gamecount, int evacount)
{
	int episodecount = 0;

	//vector<outputData> tmpd;
	ofstream resultfile;
	string filename = IntToString(gamecount) + ".csv";
	resultfile.open("Evaluation/" + IntToString(evacount) +"/"+ filename, std::ios::app);

	while (episodecount < EPISODECOUNT) {
		//視界内での状態の把握
		//敵の位置を自分との相対位置で認識
		State p1state = searchRelationEnemy(p1, enemy);
		State p2state = searchRelationEnemy(p2, enemy);

		//何ターン見ていないかという情報を反映させる．
		p1.locate_enemy_count = p1state.locate_enemy_count;
		p2.locate_enemy_count = p2state.locate_enemy_count;

		//Q値に基づく行動の選択
		int p1action = chooseAnAction(p1state, 1);
		int p2action = chooseAnAction(p2state, 2);

		//ランダムにキャラクターを動かす
		if (checkmovenemy == true) {
			enemy = protEnemyCharactor(enemy, action(engine));
		}
		else {
			enemy = protEnemyCharactor(enemy, 4);
		}

		//行動の実施
		p1 = protCharactor(p1, p1action);
		p2 = protCharactor(p2, p2action);

		if (blindcount == false) {
			p1state.locate_enemy_count = 0;
			p1.locate_enemy_count = 0;
			p2state.locate_enemy_count = 0;
			p2.locate_enemy_count = 0;
		}

		//tmpd.push_back({ p1.first,p1.second,enemy.first,enemy.second });
		resultfile << episodecount <<","<<
		p1.first <<","<<p1.second << ","<<
		p2.first <<","<<p2.second << ","<<
		enemy.first << "," << enemy.second <<endl;

		episodecount++;
		if (checkSurroundbyPlayer(p1,p2, enemy) == true) {
			break;
		}
	}
	resultfile.close();
	//		outputEvaluationMoveData(evacount, gamecount, tmpd);
	//		tmpd.clear();
	return episodecount;
}



/*********************************************************************
*
*Q�w�K�p�̃��\�b�h�R
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

//Q�l�̃e�[�u�����t�@�C���ɏo�͂���
void outputQvalueTable(int gamecount) {
	string filename1 = IntToString(gamecount) + "p1.csv";
	string filename2 = IntToString(gamecount) + "p2.csv";
	ofstream outputQvaldata1;
	outputQvaldata1.open("Qval/Qdata" + filename1, std::ios::app);

	ofstream outputQvaldata2;
	outputQvaldata2.open("Qval/Qdata" + filename2, std::ios::app);

	//Q�l��0.0�ŏ�����
	for (int m = 0; m < qSize; m++) {
		for (int i = 0; i < qSize; i++) {
			for (int j = 0; j < qSize; j++) {
				outputQvaldata1 << "," << p1Qvalue[i][j][m][2] << "," << ",";
				outputQvaldata2 << "," << p2Qvalue[i][j][m][2] << "," << ",";
			} outputQvaldata1 << endl; outputQvaldata2 << endl;
			for (int j = 0; j < qSize; j++) {
				outputQvaldata1 << p1Qvalue[i][j][m][3] << "," << p1Qvalue[i][j][m][4] << "," << p1Qvalue[i][j][m][1] << ",";
				outputQvaldata2 << p2Qvalue[i][j][m][3] << "," << p2Qvalue[i][j][m][4] << "," << p2Qvalue[i][j][m][1] << ",";
			}outputQvaldata1 << endl; outputQvaldata2 << endl;
			for (int j = 0; j < qSize; j++) {
				outputQvaldata1 << "," << p1Qvalue[i][j][m][0] << "," << ",";
				outputQvaldata2 << "," << p2Qvalue[i][j][m][0] << "," << ",";
			}outputQvaldata1 << endl; outputQvaldata2 << endl;
		}
		outputQvaldata1 << endl; outputQvaldata1 << endl;
		outputQvaldata2 << endl; outputQvaldata2 << endl;

	}
	outputQvaldata1.close();
}

int getMAXQValue(State afterstate,int playernumber){
	double maxQ = 0;
	if(playernumber == 1){
		int nextaction = getMaxQAction(afterstate, playernumber);						//after�ł̍ő�Q�l���o���s��
		maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.locate_enemy_count][nextaction];
	}

	if(playernumber == 2){
		int nextaction = getMaxQAction(afterstate, playernumber);						//after�ł̍ő�Q�l���o���s��
		maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.locate_enemy_count][nextaction];
	}

	return maxQ;
}

int MultiQlearningMethod(State p1,State p2, State enemy, int gamecount)
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
		State p2state = searchRelationEnemy(p2, enemy);

		//���^�[�����Ă��Ȃ����Ƃ��������𔽉f�������D
		p1.locate_enemy_count = p1state.locate_enemy_count;
		p2.locate_enemy_count = p2state.locate_enemy_count;

		//Q�l�Ɋ��Â��s���̑I��
		int p1action = chooseAnAction(p1state, 1);
		int p2action = chooseAnAction(p2state, 2);

		//�����_���ɃL�����N�^�[�𓮂���
		if (checkmovenemy == true) {
			enemy = protEnemyCharactor(enemy, action(engine));
		}

		//�s���̎��{
		p1 = protCharactor(p1, p1action);
		p2 = protCharactor(p2, p2action);

		//movedata1[p1.first][p1.second]++;

		//�s�������{�������̑��Έʒu���F��
		State p1afterstate = searchRelationEnemy(p1, enemy);
		State p2afterstate = searchRelationEnemy(p2, enemy);

		if (blindcount == false) {
			p1state.locate_enemy_count = 0;
			p1.locate_enemy_count = 0;
			p1afterstate.locate_enemy_count = 0;
		}

		if (MAXGAME - 50 < gamecount) {
			outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," << enemy.first << "," << enemy.second << std::endl;
			outputmovedata.close();
		}

		//calcReward
		double p1maxQ = getMAXQValue(p1afterstate, 1);
		double p2maxQ = getMAXQValue(p2afterstate, 2);

		if (checkSurroundbyPlayer(p1,p2, enemy) == true) {
			calcFinishReward(p1state, p1action, p1maxQ, AAlpha, 1);
			calcFinishReward(p2state, p2action, p2maxQ, AAlpha, 2);
			break;
		}

		if (checkNexttoEnemy(p1, enemy) == true) {
			calcSuccessReward(p1state, p1action, p1maxQ, AAlpha, 1);
		}else{
			calcFaildReward(p1state, p1action, p1maxQ, AAlpha, 1);
		}

		if (checkNexttoEnemy(p2, enemy) == true) {
			calcSuccessReward(p2state, p2action, p2maxQ, AAlpha, 2);
		}else{
			calcFaildReward(p2state, p2action, p2maxQ, AAlpha, 2);
		}

		episodecount++;
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
	int action = 4;
	double maxQ = -1.0;

	if(playernum == 1){
		for (int i = 0; i < ACTION; i++) {
			double q = p1Qvalue[state.first][state.second][state.locate_enemy_count][i];
			//�ő�Q�l�ƂȂ��s�����L��
			if (q > maxQ) {
				action = i;
				maxQ = q;
			}
		}
	}

	if(playernum == 2){
		for (int i = 0; i < ACTION; i++) {
			double q = p2Qvalue[state.first][state.second][state.locate_enemy_count][i];
			//�ő�Q�l�ƂȂ��s�����L��
			if (q > maxQ) {
				action = i;
				maxQ = q;
			}
		}
	}
	return action;
}

bool calcSuccessReward(State state, int action,double maxQ, long double AttenuationAlpha, int playernum) {
if(playernum == 1){
		p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (subrewards + ganna * maxQ);
}

if(playernum == 2){
		p2Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (subrewards + ganna * maxQ);
		}
		return true;
}

bool calcFinishReward(State state, int action,double maxQ, long double AttenuationAlpha, int playernum) {
if(playernum == 1){
		p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (rewards + ganna * maxQ);
}

if(playernum == 2){
		p2Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (rewards + ganna * maxQ);
		}
		return true;
}

bool calcFaildReward(State state, int action,double maxQ, long double AttenuationAlpha, int playernum) {
	if(playernum == 1){
			p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (faild + ganna * maxQ);
	}

	if(playernum == 2){
			p2Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p2Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (faild + ganna * maxQ);
			}
			return true;
	}
