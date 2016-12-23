#include "Header.h"

using namespace std;

// メルセンヌ・ツイスター法による擬似乱数生成器を、
// ハードウェア乱数をシードにして初期化
random_device seed_gen;
mt19937 engine(seed_gen());
std::uniform_int_distribution<> dist1(0, mapsize);
std::uniform_int_distribution<> action(0, ACTION);
std::uniform_int_distribution<> makerandom(0, 100);

//const以外の数値を.hで定義すると怒られるので
int outputcount = 100000;
double p1Qvalue[qSize][qSize][qSize][ACTION] ={0};
double p2Qvalue[qSize][qSize][qSize][ACTION] ={0};
unsigned int map[mapsize][mapsize] ={0};

int main() {
//	checkExistenceOfFolder("Evaluation/");
//	checkExistenceOfFolder("moveData/");
//	checkExistenceOfFolder("Qval/");
//	checkExistenceOfFolder("Result/");

	State p1 = initState(dist1(engine), dist1(engine));
	State p2 = initState(dist1(engine), dist1(engine));
	State enemy = initState(dist1(engine), dist1(engine));

	setPlayer(p1);
	setPlayer(p2);
	setEnemy(enemy);

	initializeQvalue();

	ofstream resultfile;
	string filename = "Result.txt";
	resultfile.open("Result/" + filename, std::ios::app);

	int gamecount = 0;
	int episodecount = 0;
	//メインループ
	while (gamecount < MAXGAME) {
		episodecount = QlearningMethod(p1, p2, enemy, gamecount);
		resetmap();

		while ((p1.first == enemy.first && p1.second == enemy.second) || p1.first == p2.first && p1.second && p2.second) {
			p1 = initState(dist1(engine), dist1(engine));
			p2 = initState(dist1(engine), dist1(engine));
			enemy = initState(dist1(engine), dist1(engine));
		}
		setPlayer(p1);
		setPlayer(p2);
		setEnemy(enemy);

		resultfile << gamecount << "," << episodecount << std::endl;

		//評価用
		if (gamecount == outputcount) {
			outputQvalueTable(gamecount);
			outputEvaluationQvalueTable(gamecount);
			EvaluationFunction(gamecount);
			//桁が一つ上がる度に記録する
			outputcount = outputcount * 10;
		}
		gamecount++;
	}
	//Qvalの出力
	outputQvalueTable(gamecount);
	outputEvaluationQvalueTable(gamecount);
	EvaluationFunction(gamecount);
	resultfile.close();

	return 0;
}


void EvaluationFunction(int evacount) {
	//checkExistenceOfFolder("Evaluation/" + to_string(evacount));
	random_device seed_gen;
	mt19937 engine(seed_gen());
	std::uniform_int_distribution<> dist1(0, mapsize);

	int gamecount = 0;
	int episodecount = 0;
	State evalp1 = initState(dist1(engine), dist1(engine));
	State evalp2 = initState(dist1(engine), dist1(engine));
	State evalenemy = initState(dist1(engine), dist1(engine));

	ofstream evalresultfile;
	string evalfilename = "Result.txt";
	evalresultfile.open("Evaluation/" + to_string(evacount) + "/" + evalfilename, std::ios::app);

	while (gamecount < EVALUATIONCOUNT) {
		//do {
		evalp1 = initState(dist1(engine), dist1(engine));
		evalp2 = initState(dist1(engine), dist1(engine));
		evalenemy = initState(dist1(engine), dist1(engine));
		//} while (evalp1.first == evalenemy.first && evalp1.second == evalenemy.second);

		setPlayer(evalp1);
		setPlayer(evalp2);
		setEnemy(evalenemy);

		episodecount = QlearningEvaluationMethod(evalp1,evalp2, evalenemy, gamecount,evacount);
		resetmap();

		evalresultfile << gamecount << "," << episodecount << std::endl;
		gamecount++;
	}
	evalresultfile.close();
}
void outputEvaluationQvalueTable(int evacount) {
	ofstream outputQvaldata1;
	string filename = "EvalQdata.csv";

	//checkExistenceOfFolder("Evaluation/" + to_string(evacount));
	outputQvaldata1.open("Evaluation/" + to_string(evacount) + "/Qdata" + filename, std::ios::app);
	const int dx[] = { 0,1,0,-1,0 };
	const int dy[] = { -1,0,1,0,0 };
	for (int m = 0; m < qSize; m++) {
		for (int i = 0; i < qSize; i++) {
			for (int j = 0; j < qSize; j++) {
				for (int action = 0; action < qSize; action++) {
					outputQvaldata1 << p1Qvalue[i][j][m][action] << ",";
				}		outputQvaldata1 << endl;
			}outputQvaldata1 << endl;
		}
	}
}
//Mapの初期化
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
//マップの表示
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

//敵の隣にプレイヤがいるかどうかを調べる.
bool checkNexttoEnemy(State player, State enemy) {
	const int dx[] = { 0,1,0,-1,0 };
	const int dy[] = { -1,0,1,0,0 };

	for (int action = 0; action < 5; action++) {
		State tmp = checkCharacter(player, action);
		if (enemy.first == tmp.first && enemy.second == tmp.second) {
			return true;
		}
	}
	return false;
}

//敵を囲むことが出来ているか調べる
bool checkSurroundbyPlayer(State player1, State player2, State enemy) {

	bool check1 = checkNexttoEnemy(player1, enemy);
	bool check2 = checkNexttoEnemy(player2, enemy);

	if (check1 == true && check2 == true) {
		return true;
	}
	return false;
}

//キャラクターを取り囲むことが出来るかどうかの確認用メソッド
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
	//cout << character.first << " " << character.second << endl;

	return character;
}

//characterの移動,mapに表示するための書き込みを行うところ.
State protCharactor(State player, int action) {
	resetPlayer(player);
	player = moveCharacter(player, action);
	setPlayer(player);
	return player;
}

//敵のキャラクターを移動させる
State protEnemyCharactor(State player, int action) {
	resetPlayer(player);
	player = moveCharacter(player, action);
	setEnemy(player);
	return player;
}

//キャラクターを移動させ,移動することが出来ればその移動先の座標を返す
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

	//移動不可能な場合戻る.
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
*Q学習用のメソッド軍
*
**********************************************************************/
//Qテーブルの初期化
void initializeQvalue() {
	//Q値を0.0で初期化
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

//Q値のテーブルをファイルに出力する
void outputQvalueTable(int gamecount) {
	ofstream outputQvaldata1;
	string filename = to_string(gamecount) + "p1.csv";
	outputQvaldata1.open("Qval/Qdata" + filename, std::ios::app);

	ofstream outputQvaldata2;
	string filename2 = to_string(gamecount) + "p2.csv";
	outputQvaldata2.open("Qval/Qdata" + filename2, std::ios::app);

	const int dx[] = { 0,1,0,-1,0 };
	const int dy[] = { -1,0,1,0,0 };
	//Q値を0.0で初期化
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
}


//Q学習用
int QlearningMethod(State p1,State p2, State enemy, int gamecount)
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

		//行動の実施
		p1 = protCharactor(p1, p1action);
		State p1afterstate = searchRelationEnemy(p1, enemy);

		p2 = protCharactor(p2, p1action);
		State p2afterstate = searchRelationEnemy(p2, enemy);


		if (blindcount == false) {
			p1state.locate_enemy_count = 0;
			p1.locate_enemy_count = 0;
			p1afterstate.locate_enemy_count = 0;

			p2state.locate_enemy_count = 0;
			p2.locate_enemy_count = 0;
			p2afterstate.locate_enemy_count = 0;
		}

		//報酬の付与
		calcReward(p1state, p1afterstate, p2state,p2afterstate, p1action, p2action, p1, p2 , enemy, AAlpha);

		//ラスト50ゲームのファイルだけ出力
		outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," <<p2.first <<"," << p2.second <<"," << enemy.first << "," << enemy.second << std::endl;

		//drawMap();
		//ゲームの修了判定
		episodecount++;
		if (checkSurroundbyPlayer(p1,p2,enemy) == true) {
			break;
		}
	}
	return episodecount;
}

//評価用
int QlearningEvaluationMethod(State p1,State p2, State enemy, int gamecount,int evacount)
{
	int episodecount = 0;
	ofstream outputmovedata;
	string movedatafilename = to_string(gamecount) + ".csv";
	outputmovedata.open("Evaluation/" + to_string(evacount) + "/" + movedatafilename, std::ios::app);

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

		//行動の実施
		p1 = protCharactor(p1, p1action);
		State p1afterstate = searchRelationEnemy(p1, enemy);

		p2 = protCharactor(p2, p1action);
		State p2afterstate = searchRelationEnemy(p2, enemy);


		if (blindcount == false) {
			p1state.locate_enemy_count = 0;
			p1.locate_enemy_count = 0;
			p1afterstate.locate_enemy_count = 0;

			p2state.locate_enemy_count = 0;
			p2.locate_enemy_count = 0;
			p2afterstate.locate_enemy_count = 0;
		}

		//ラスト50ゲームのファイルだけ出力
		outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," << p2.first << "," << p2.second << "," << enemy.first << "," << enemy.second << std::endl;

		//drawMap();
		//ゲームの修了判定
		episodecount++;
		if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
			break;
		}
	}
	return episodecount;
}


//敵と自機の視野内での相対距離を算出する.
//視野外の場合視界の中央となる点＝自機を座標データとして返す.
State searchRelationEnemy(State playerpositions, State enemypositons) {
	//std::cout << "searchRelationEnemy" << std::endl;
	State ep;
	State tmp = { 0,0 };

	ep.first = enemypositons.first - playerpositions.first;
	ep.second = enemypositons.second - playerpositions.second;
	ep.locate_enemy_count = playerpositions.locate_enemy_count;

	//自機が敵より左
	if (playerpositions.first < enemypositons.first) {
		tmp.first = (enemypositons.first - mapsize) - playerpositions.first;
	}
	else {
		tmp.first = (mapsize - playerpositions.first) + enemypositons.first;
	}

	//自機が敵より下
	if (playerpositions.second < enemypositons.second) {
		tmp.second = (enemypositons.second - mapsize) - playerpositions.second;
	}
	else {
		tmp.second = (mapsize - playerpositions.second) + enemypositons.second;
	}

	//絶対値で距離を計算する
	if (abs(tmp.first) < abs(ep.first)) {
		ep.first = tmp.first;
	}

	if (abs(tmp.second) < abs(ep.second)) {
		ep.second = tmp.second;
	}

	//視野外の時の座標では全て固定.
	if (abs(ep.first) > eyesight) {
		ep.first = eyesight;
		ep.second = eyesight;
		//視野外のため見えてないカウントを追加
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

	//相対位置に視界分を足すことでQ値を格納する位置を調整する.
	ep.first = ep.first + eyesight;
	ep.second = ep.second + eyesight;

	ep.locate_enemy_count = 0;

	return ep;
}

//Q値によって決定付けされる行動を選択する.
//ε-グリーディ法
int chooseAnAction(State playerstate, int playernum) {
	int randvalue = makerandom(engine);
	//行動を選択
	int action;
	if (randvalue < EPSILON) {
		//ランダム選択
		action = randvalue % 5;
	}
	else {
		//通常は、Q値を最大化する行動を選択
		action = getMaxQAction(playerstate, playernum);
	}
	return action;
}

//状態stateにおいて、最大のQ値となる行動を返す
int getMaxQAction(State state, int playernum) {
	double maxQ = -1.0;
	int action = 4;
	if (playernum == 1) {
		for (int i = 0; i < ACTION; i++) {
			double q = p1Qvalue[state.first][state.second][state.locate_enemy_count][i];

			//最大Q値となる行動を記憶
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
	}

	if (playernum == 2) {
		for (int i = 0; i < ACTION; i++) {
			double q = p2Qvalue[state.first][state.second][state.locate_enemy_count][i];

			//最大Q値となる行動を記憶
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
	}
	return action;
}

//評価用移動メソッド
int chooseAnEvaluationAction(State playerstate, int playernum) {
	//�ʏ��́AQ�l���ő剻�����s�����I��
	int	action = getMaxQAction(playerstate, playernum);

	return action;
}

//報酬関数
bool calcReward(State p1state, State p1afterstate, State p2state, State p2afterstate, int action,int action2, State p1,State p2, State enemy, long double AttenuationAlpha) {
	double maxQ1 = 0;
	double maxQ2 = 0;
	int p1nextaction = getMaxQAction(p1afterstate, 1);						//afterでの最大Q値を出す行動
	int p2nextaction = getMaxQAction(p2afterstate, 2);						//afterでの最大Q値を出す行動
	maxQ1 = p1Qvalue[p1afterstate.first][p1afterstate.second][p1afterstate.locate_enemy_count][p1nextaction];	//afterでの最大Q値
	maxQ2 = p2Qvalue[p2afterstate.first][p2afterstate.second][p2afterstate.locate_enemy_count][p2nextaction];	//afterでの最大Q値


	//Surround Enemy																											//はさむことが出来れば追加
	if (checkSurroundbyPlayer(p1, p2, enemy) == true) {
		p1Qvalue[p1state.first][p1state.second][p1state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[p1state.first][p1state.second][p1state.locate_enemy_count][action] + AttenuationAlpha* (rewards + ganna * maxQ1);
		p2Qvalue[p2state.first][p2state.second][p2state.locate_enemy_count][action2] = (1 - AttenuationAlpha)*p2Qvalue[p2state.first][p2state.second][p2state.locate_enemy_count][action2] + AttenuationAlpha* (rewards + ganna * maxQ2);
		return true;
	}

	//Player1
	//if (checkNexttoEnemy(p1, enemy) == true) {
	//	p1Qvalue[p1state.first][p1state.second][p1state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[p1state.first][p1state.second][p1state.locate_enemy_count][action] + AttenuationAlpha* (subrewards + ganna * maxQ1);
	//}
	//
	////Player2
	//if (checkNexttoEnemy(p2, enemy) == true) {
	//	p2Qvalue[p2state.first][p2state.second][p2state.locate_enemy_count][action2] = (1 - AttenuationAlpha)*p2Qvalue[p2state.first][p2state.second][p2state.locate_enemy_count][action2] + AttenuationAlpha* (subrewards + ganna * maxQ2);
	//}

	p1Qvalue[p1state.first][p1state.second][p1state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[p1state.first][p1state.second][p1state.locate_enemy_count][action] + AttenuationAlpha* (faild + ganna * maxQ1);
	p2Qvalue[p2state.first][p2state.second][p2state.locate_enemy_count][action2] = (1 - AttenuationAlpha)*p2Qvalue[p2state.first][p2state.second][p2state.locate_enemy_count][action2] + AttenuationAlpha* (faild + ganna * maxQ2);
	return false;
}
