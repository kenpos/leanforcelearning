#include "Header.h"

using namespace std;

// メルセンヌ・ツイスター法による擬似乱数生成器を、
// ハードウェア乱数をシードにして初期化
random_device seed_gen;
mt19937 engine(seed_gen());
std::uniform_int_distribution<> dist1(0, mapsize);
std::uniform_int_distribution<> action(0, ACTION);
std::uniform_int_distribution<> makerandom(0, 100);

int main() {
	State p1 = initState(dist1(engine), dist1(engine));
	//State p2 =  initState(dist1(engine), (int)dist1(engine));
	State enemy = initState(dist1(engine), dist1(engine));

	setPlayer(p1);
	//setPlayer(p2);
	setEnemy(enemy);

	initializeQvalue();
	//フォルダが無ければ作成

	ofstream resultfile;
	string filename = "Result.txt";
	resultfile.open("Result/" + filename, std::ios::app);

	int gamecount = 0;
	int episodecount = 0;
	//メインループ
	while (gamecount < MAXGAME) {
		//int action;
		//int b;
		//cin >> action;
		//cin >> b;

		//p1 = protCharactor(p1, action);
		//p2 = protCharactor(p2, b);
		//State p1state = searchRelationEnemy(p1, enemy);
		//p1.locate_enemy_count = p1state.locate_enemy_count;

		//episodecount = QlearningMethod(p1, p2, enemy,gamecount);

		episodecount = SoloQlearningMethod(p1, enemy, gamecount);
		resetmap();

		while (p1.first == enemy.first && p1.second == enemy.second) {
			p1 = initState(dist1(engine), dist1(engine));
			enemy = initState(dist1(engine), dist1(engine));
			//	p2 = { (int)dist1(engine), (int)dist1(engine) };
		}
		setPlayer(p1);
		//setPlayer(p2);
		setEnemy(enemy);

		//drawMap();

		resultfile << gamecount << "," << episodecount << std::endl;
		gamecount++;
	}
	//Qvalの出力
	outputQvalueTable();
	resultfile.close();


	return 0;
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

//その地点に何回到達したのか回数を表示する.
//初期化してるところ
//void initializeMoveData() {
//	//Q値を0.0で初期化
//	for (int i = 0; i < mapsize; i++) {
//		for (int j = 0; j < mapsize; j++) {
//				movedata1[i][j] = 0;
//				movedata2[i][j] = 0;
//			}
//		}
//	}


//Q値のテーブルをファイルに出力する
void outputQvalueTable() {
	ofstream outputQvaldata1;
	string filename = ".csv";

	outputQvaldata1.open("Qval/Qdata" + filename, std::ios::app);
	const int dx[] = { 0,1,0,-1,0 };
	const int dy[] = { -1,0,1,0,0 };
	//Q値を0.0で初期化
	for (int m = 0; m < qSize; m++) {
		for (int i = 0; i < qSize; i++) {

			for (int j = 0; j < qSize; j++) {
				outputQvaldata1 << "," << p1Qvalue[i][j][m][2] << "," << ",";
			} outputQvaldata1 << endl;
			for (int j = 0; j < qSize; j++) {
				outputQvaldata1 << p1Qvalue[i][j][m][3] << "," << p1Qvalue[i][j][m][4] << "," << p1Qvalue[i][j][m][1] << ",";
			}outputQvaldata1 << endl;
			for (int j = 0; j < qSize; j++) {
				outputQvaldata1 << "," << p1Qvalue[i][j][m][0] << "," << ",";
			}outputQvaldata1 << endl;
		}
		outputQvaldata1 << endl; outputQvaldata1 << endl;
	}
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
//	//Q値を0.0で初期化
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


//1エピソードを再現する.
/*
(1)エージェントは環境の状態sを観測する
(2)任意の行動選択をし，行動aを実行する
(3)環境から報酬を受け取る
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
//		//視界内での状態の把握
//		//敵の位置を自分との相対位置で認識
//		State p1state = searchRelationEnemy(p1, enemy);
//		State p2state = searchRelationEnemy(p2, enemy);
//
//		//Q値に基づく行動の選択
//		int p1action = chooseAnAction(p1state,p1,1);
//		int p2action = chooseAnAction(p2state,p2,2);
//
//		//行動の実施
//		p1 = protCharactor(p1, p1action);
//		movedata1[p1.first][p1.second]++;
//		p2 = protCharactor(p2, p2action);
//		movedata2[p2.first][p2.second]++;
//
//		//行動を実施した後の相対位置を認識
//		State p1afterstate = searchRelationEnemy(p1, enemy);
//		State p2afterstate = searchRelationEnemy(p2, enemy);
//
//		//報酬の付与
//		calcReward(p1state, p1afterstate, p1action, p1, p2, enemy, 1, AttenuationAlpha);
//		calcReward(p2state, p2afterstate, p2action, p1, p2, enemy, 2, AttenuationAlpha);
//
//		//ファイル出力
//		if (MAXGAME -50 < gamecount) {
//			ofstream outputmovedata;
//			outputmovedata.open(".\\" + foldaname + "\\moveData\\" + filename, std::ios::app);
//			outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," << p2.first << "," << p2.second << "," << enemy.first << "," << enemy.second << std::endl;
//		}
//		//drawMap();
//
//		//ゲームの修了判定
//		episodecount++;
//		if (checkSurroundbyPlayer(p1, p2, enemy) == true) {//二人用
//		break;
//		}
//	}
//	//std::cout << "GAME:" <<gamecount << " "<< episodecount << " 終了！" << endl;
//	
//	outputDoublePlayQvalueTable();
//
//	//MoveCountの出力
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


int SoloQlearningMethod(State p1, State enemy, int gamecount)
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

		//何ターン見ていないかという情報を反映させる．
		p1.locate_enemy_count = p1state.locate_enemy_count;

		//Q値に基づく行動の選択
		int p1action = chooseAnAction(p1state, 1);

		//ランダムにキャラクターを動かす
		if (checkmovenemy == true) {
			enemy = protEnemyCharactor(enemy, action(engine));
		}

		//行動の実施
		p1 = protCharactor(p1, p1action);
		//movedata1[p1.first][p1.second]++;

		//行動を実施した後の相対位置を認識
		State p1afterstate = searchRelationEnemy(p1, enemy);
		if (blindcount == false) {
			p1state.locate_enemy_count = 0;
			p1.locate_enemy_count = 0;
			p1afterstate.locate_enemy_count = 0;
		}
		//報酬の付与
		calcSoloReward(p1state, p1afterstate, p1action, p1, enemy, AAlpha);

		//ラスト50ゲームのファイルだけ出力
		outputmovedata << episodecount << "," << p1.first << "," << p1.second << "," << enemy.first << "," << enemy.second << std::endl;


		//drawMap();
		//ゲームの修了判定
		episodecount++;
		if (checkNexttoEnemy(p1, enemy) == true) {
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


//報酬関数
bool calcSoloReward(State state, State afterstate, int action, State player, State enemy, long double AttenuationAlpha) {
	double maxQ = 0;
	//絶対座標で移動した状態を取得する
	//std::Statestate =  moveCharacter(player, action);	//aをした後の状態
	//std::Stateafter = searchRelationEnemy(player, enemy); //相対座標に変換
	int nextaction = getMaxQAction(afterstate, 1);						//afterでの最大Q値を出す行動
	maxQ = p1Qvalue[afterstate.first][afterstate.second][afterstate.locate_enemy_count][nextaction];			//afterでの最大Q値
	if (checkNexttoEnemy(player, enemy) == true) {
		p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (rewards + gamma * maxQ);
		return true;
	}
	p1Qvalue[state.first][state.second][state.locate_enemy_count][action] = (1 - AttenuationAlpha)*p1Qvalue[state.first][state.second][state.locate_enemy_count][action] + AttenuationAlpha* (faild + gamma * maxQ);
	return false;
}