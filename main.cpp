#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <ctime>
#include <conio.h>
#include <graphics.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct text
{
	TCHAR str[1000];
	int x1, y1, x2, y2;
	int size;
}textbox[500][2],aibox[5];
struct board_
{
	int top, bottom,right, left;
	int boardLen,gridLen;
}myBoard,miniBoard[3];
struct point
{
	int x;
	int y;
	int val;
};

const int SCREEN_LEN = 1000;
const int SCREEN_WID = 600;
const int color1 = 20;
const int color2 = 220;
const int menuTextSize = 40;
const int menuTextSize1 = 45;
const int menuTextSize2 = 50;
const int aiTextSize = 45;
const int aiTextSizeMid = 37;
const int aiTextSizeSmall = 35;
const int sleepTime = 100;

int board[11][11];	// -1 for none ; 1 for black ; 0 for white;
bool searchBoard[11][11];
const int DIRX[4] = { -1,0,1,0 };
const int DIRY[4] = { 0,1,0,-1 };
bool bot_piece;
bool man_piece;
bool now_piece;
int myCount = 0;
int test = 0;
int global_avail;
clock_t timeStart;
int saveBoard[3][11][11];
int saveBot[3];
bool save_man_piece[3];
int saveCount[3];

int ai_setting=-1;
COLORREF BKCOLOR = RGB(color1, color1, color1);
COLORREF FTCOLOR = RGB(color2, color2, color2);

void _init_text();
void _start();
void _init_menu();
void _help();
int _choose_ai();
int _choose_first();
void _init_board();
void _draw_board(board_ myBoard,int board[11][11],double timeControl);
void put_piece(int i, int j, bool piece);
bool judge_pos(MOUSEMSG msg,text t);
void main_loop();
point get_mouse(MOUSEMSG msg);
void redraw_line(point tmp);
void win_lose(bool bot_win,bool man_win);
void _init_saveload();
int choose_save();
void write(int choice);
void strcpy_T(TCHAR* dst, const TCHAR* src);

void initialize();
bool search_breath(int i, int j, bool piece);
bool is_legal(int i, int j, bool move);
int count_avail(bool piece);
int val_state(bool piece);
int minimax(int i, int j, bool piece, int layer);
int max_val(bool piece, int layer);
double MCTS(int testNum, double timelimit, bool bot_piece);
point ai_1(bool piece);
point ai_2(bool piece);
point ai_3(bool piece);
point ai_4(bool piece);

int main()
{
	srand((unsigned)time(0));
	initialize();
	initgraph(SCREEN_LEN, SCREEN_WID);
	setbkcolor(BKCOLOR);
	_init_text();
	_start();
	_init_menu();
	bool stay = true;
	bool touch[10] = { 0 };
	bool touchElse[10] = { 0 };
	while (stay)
	{
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			FlushMouseMsgBuffer();
			if (msg.mkLButton && judge_pos(msg, textbox[2][0]))
			{
				Sleep(sleepTime);
				ai_setting=_choose_ai();
				Sleep(sleepTime);
				if (ai_setting == 5)
					_init_menu();
				else
				{
					int choice = _choose_first();
					Sleep(sleepTime);
					if (choice == -1)
						_init_menu();
					else
					{
						man_piece = choice;
						bot_piece = !man_piece;
						now_piece = 1;
						_init_board();
						initialize();
						main_loop();
					}
				}
			}
			if (msg.mkLButton && judge_pos(msg, textbox[3][0]))
			{
				Sleep(sleepTime);
				_init_saveload();
				int choice = choose_save();
				if (choice == 3)
					_init_menu();
				else
				{
					ai_setting = saveBot[choice];
					man_piece = save_man_piece[choice];
					bot_piece = !man_piece;
					now_piece = !(saveCount[choice]%2);
					memcpy(board, saveBoard[choice], sizeof(board));
					for (int i = 0; i < 11; ++i)
					{
						board[0][i] = -2;
						board[10][i] = -2;
						board[i][0] = -2;
						board[i][10] = -2;
					}
					_init_board();
					main_loop();
				}
			}
			if (msg.mkLButton && judge_pos(msg, textbox[4][0]))
			{
				Sleep(sleepTime);
				_help();
			}
			if (msg.mkLButton && judge_pos(msg, textbox[5][0]))
				stay = false;
			for (int i = 2; i <= 5; ++i)
			{
				if (judge_pos(msg, textbox[i][0]))
				{
					touch[i] = true;
					touchElse[i] = false;
					settextstyle(menuTextSize1, 0, _T("微软雅黑"));
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].x2, textbox[i][0].y2);
					outtextxy(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].str);
				}
				else
					touchElse[i] = true;
				if (touch[i] && touchElse[i])
				{
					touch[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].x2, textbox[i][1].y2);
					settextstyle(menuTextSize, 0, _T("微软雅黑"));
					outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
				}
			}
		}
	}
	closegraph();
	return 0;
}

// UI Function

void _init_text()
{
	strcpy_T(textbox[0][0].str, _T("Welcome To NoGo"));
	settextstyle(80, 0, _T("微软雅黑"));
	textbox[0][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[0][0].str) / 2;
	textbox[0][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[0][0].str) / 2;
	textbox[0][0].y1 = 0.5 * SCREEN_WID - textheight(textbox[0][0].str);
	textbox[0][0].y2 = 0.5 * SCREEN_WID;

	strcpy_T(textbox[1][0].str, _T("不围棋"));
	settextstyle(70, 0, _T("微软雅黑"));
	textbox[1][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[1][0].str) / 2;
	textbox[1][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[1][0].str) / 2;
	textbox[1][0].y1 = 0.12 * SCREEN_WID;
	textbox[1][0].y2 = 0.12 * SCREEN_WID + textheight(textbox[1][0].str);

	strcpy_T(textbox[2][0].str, _T("新游戏"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[2][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[2][0].str) / 2;
	textbox[2][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[2][0].str) / 2;
	textbox[2][0].y1 = 0.35 * SCREEN_WID;
	textbox[2][0].y2 = 0.35 * SCREEN_WID + textheight(textbox[2][0].str);

	strcpy_T(textbox[3][0].str, _T("读取进度"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[3][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[3][0].str) / 2;
	textbox[3][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[3][0].str) / 2;
	textbox[3][0].y1 = 0.45 * SCREEN_WID;
	textbox[3][0].y2 = 0.45 * SCREEN_WID + textheight(textbox[3][0].str);

	strcpy_T(textbox[4][0].str, _T("帮助"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[4][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[4][0].str) / 2;
	textbox[4][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[4][0].str) / 2;
	textbox[4][0].y1 = 0.55 * SCREEN_WID;
	textbox[4][0].y2 = 0.55 * SCREEN_WID + textheight(textbox[4][0].str);

	strcpy_T(textbox[5][0].str, _T("退出"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[5][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[5][0].str) / 2;
	textbox[5][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[5][0].str) / 2;
	textbox[5][0].y1 = 0.65 * SCREEN_WID;
	textbox[5][0].y2 = 0.65 * SCREEN_WID + textheight(textbox[5][0].str);

	strcpy_T(textbox[6][0].str, _T("返回"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[6][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[6][0].str) / 2;
	textbox[6][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[6][0].str) / 2;
	textbox[6][0].y1 = 0.88 * SCREEN_WID;
	textbox[6][0].y2 = 0.88 * SCREEN_WID + textheight(textbox[6][0].str);

	strcpy_T(textbox[7][0].str, _T("  不围棋由围棋衍生而来，属于两人零和完备信息博弈，规则与围棋相反。"));
	settextstyle(30, 0, _T(" 微软雅黑"));
	textbox[7][0].x1 = 0.1 * SCREEN_LEN;
	textbox[7][0].x2 = textbox[7][0].x1 + textwidth(textbox[7][0].str);
	textbox[7][0].y1 = 0.065 * SCREEN_WID;
	textbox[7][0].y2 = textbox[7][0].y1 + textheight(textbox[7][0].str);

	strcpy_T(textbox[8][0].str, _T(" 规则如下： "));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[8][0].x1 = 0.1 * SCREEN_LEN;
	textbox[8][0].x2 = textbox[8][0].x1 + textwidth(textbox[8][0].str);
	textbox[8][0].y1 = textbox[7][0].y2;
	textbox[8][0].y2 = textbox[8][0].y1 + textheight(textbox[8][0].str);

	strcpy_T(textbox[9][0].str, _T("  1. 棋盘同九路围棋棋盘，9×9"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[9][0].x1 = 0.1 * SCREEN_LEN;
	textbox[9][0].x2 = textbox[9][0].x1 + textwidth(textbox[9][0].str);
	textbox[9][0].y1 = textbox[8][0].y2 + 0.02 * SCREEN_WID;
	textbox[9][0].y2 = textbox[9][0].y1 + textheight(textbox[9][0].str);

	strcpy_T(textbox[10][0].str, _T("  2. 黑子先手，双方轮流落子，落子后棋子不可移动。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[10][0].x1 = 0.1 * SCREEN_LEN;
	textbox[10][0].x2 = textbox[10][0].x1 + textwidth(textbox[10][0].str);
	textbox[10][0].y1 = textbox[9][0].y2;
	textbox[10][0].y2 = textbox[10][0].y1 + textheight(textbox[10][0].str);

	strcpy_T(textbox[11][0].str, _T("  3. 你不能吃掉对方的棋子。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[11][0].x1 = 0.1 * SCREEN_LEN;
	textbox[11][0].x2 = textbox[11][0].x1 + textwidth(textbox[11][0].str);
	textbox[11][0].y1 = textbox[10][0].y2;
	textbox[11][0].y2 = textbox[11][0].y1 + textheight(textbox[11][0].str);

	strcpy_T(textbox[12][0].str, _T("  4. 你不能自杀或空手。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[12][0].x1 = 0.1 * SCREEN_LEN;
	textbox[12][0].x2 = textbox[12][0].x1 + textwidth(textbox[12][0].str);
	textbox[12][0].y1 = textbox[11][0].y2;
	textbox[12][0].y2 = textbox[12][0].y1 + textheight(textbox[12][0].str);

	strcpy_T(textbox[13][0].str, _T("  5. 最后无棋可下的人判负，对弈结果只有胜负，没有和棋。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[13][0].x1 = 0.1 * SCREEN_LEN;
	textbox[13][0].x2 = textbox[13][0].x1 + textwidth(textbox[13][0].str);
	textbox[13][0].y1 = textbox[12][0].y2;
	textbox[13][0].y2 = textbox[13][0].y1 + textheight(textbox[13][0].str);

	strcpy_T(textbox[14][0].str, _T("  6. 吃子定义：一个棋子在棋盘上，与它直线紧邻的空点是这个棋子的“气”。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[14][0].x1 = 0.1 * SCREEN_LEN;
	textbox[14][0].x2 = textbox[14][0].x1 + textwidth(textbox[14][0].str);
	textbox[14][0].y1 = textbox[13][0].y2;
	textbox[14][0].y2 = textbox[14][0].y1 + textheight(textbox[14][0].str);

	strcpy_T(textbox[15][0].str, _T("    棋子直线紧邻的点上，如果有同色棋子存在，则它们便相互连接成一个不可"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[15][0].x1 = 0.1 * SCREEN_LEN;
	textbox[15][0].x2 = textbox[15][0].x1 + textwidth(textbox[15][0].str);
	textbox[15][0].y1 = textbox[14][0].y2;
	textbox[15][0].y2 = textbox[15][0].y1 + textheight(textbox[15][0].str);


	strcpy_T(textbox[16][0].str, _T("      分割的整体。它们的气也应一并计算。 棋子直线紧邻的点上，如果有异色棋 "));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[16][0].x1 = 0.1 * SCREEN_LEN;
	textbox[16][0].x2 = textbox[16][0].x1 + textwidth(textbox[16][0].str);
	textbox[16][0].y1 = textbox[15][0].y2;
	textbox[16][0].y2 = textbox[16][0].y1 + textheight(textbox[16][0].str);

	strcpy_T(textbox[17][0].str, _T("     子存在，它们之间的直线相邻的点称为“气点”，整体的“气”也应一并计算。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[17][0].x1 = 0.1 * SCREEN_LEN;
	textbox[17][0].x2 = textbox[17][0].x1 + textwidth(textbox[17][0].str);
	textbox[17][0].y1 = textbox[16][0].y2;
	textbox[17][0].y2 = textbox[17][0].y1 + textheight(textbox[17][0].str);


	strcpy_T(textbox[18][0].str, _T("     无气状态的棋子不能在棋盘上存在。按照不围棋的规则，你的落手不能导致"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[18][0].x1 = 0.1 * SCREEN_LEN;
	textbox[18][0].x2 = textbox[18][0].x1 + textwidth(textbox[18][0].str);
	textbox[18][0].y1 = textbox[17][0].y2;
	textbox[18][0].y2 = textbox[18][0].y1 + textheight(textbox[18][0].str);

	strcpy_T(textbox[19][0].str, _T("     自己 或对手的棋子陷入无气状态。"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[19][0].x1 = 0.1 * SCREEN_LEN;
	textbox[19][0].x2 = textbox[19][0].x1 + textwidth(textbox[19][0].str);
	textbox[19][0].y1 = textbox[18][0].y2;
	textbox[19][0].y2 = textbox[19][0].y1 + textheight(textbox[19][0].str);

	strcpy_T(textbox[20][0].str, _T("  祝您游戏愉快！"));
	settextstyle(30, 0, _T("微软雅黑"));
	textbox[20][0].x1 = 0.1 * SCREEN_LEN;
	textbox[20][0].x2 = textbox[20][0].x1 + textwidth(textbox[20][0].str);
	textbox[20][0].y1 = textbox[19][0].y2 + 0.02 * SCREEN_WID;
	textbox[20][0].y2 = textbox[20][0].y1 + textheight(textbox[20][0].str);

	strcpy_T(textbox[21][0].str, _T("Designed by lbx"));
	settextstyle(menuTextSize1, 0, _T("微软雅黑"));
	textbox[21][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[21][0].str) / 2;
	textbox[21][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[21][0].str) / 2;
	textbox[21][0].y1 = 0.5 * SCREEN_WID - textheight(textbox[21][0].str);
	textbox[21][0].y2 = 0.5 * SCREEN_WID;

	strcpy_T(textbox[22][0].str, _T("请选择你的对手："));
	settextstyle(menuTextSize1, 0, _T("微软雅黑"));
	textbox[22][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[22][0].str) / 2;
	textbox[22][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[22][0].str) / 2;
	textbox[22][0].y1 = 0.1 * SCREEN_WID - textheight(textbox[22][0].str);
	textbox[22][0].y2 = 0.1 * SCREEN_WID;

	strcpy_T(textbox[23][0].str, _T("NoGo_Random : 可可爱爱，没有脑袋"));
	settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
	textbox[23][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[23][0].str) / 2;
	textbox[23][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[23][0].str) / 2;
	textbox[23][0].y1 = 0.26 * SCREEN_WID - textheight(textbox[23][0].str);
	textbox[23][0].y2 = 0.26 * SCREEN_WID;

	strcpy_T(textbox[24][0].str, _T("NoGo_Simple : 年轻且天真， 有时很单纯"));
	settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
	textbox[24][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[24][0].str) / 2;
	textbox[24][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[24][0].str) / 2;
	textbox[24][0].y1 = 0.42 * SCREEN_WID - textheight(textbox[24][0].str);
	textbox[24][0].y2 = 0.42 * SCREEN_WID;

	strcpy_T(textbox[25][0].str, _T("NoGo_Killer : 击败了99%的玩家"));
	settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
	textbox[25][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[25][0].str) / 2;
	textbox[25][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[25][0].str) / 2;
	textbox[25][0].y1 = 0.58 * SCREEN_WID - textheight(textbox[25][0].str);
	textbox[25][0].y2 = 0.58 * SCREEN_WID;

	strcpy_T(textbox[26][0].str, _T("NoGo_God : 柯洁来了也不行"));
	settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
	textbox[26][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[26][0].str) / 2;
	textbox[26][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[26][0].str) / 2;
	textbox[26][0].y1 = 0.75 * SCREEN_WID - textheight(textbox[26][0].str);
	textbox[26][0].y2 = 0.75 * SCREEN_WID;

	strcpy_T(textbox[27][0].str, _T("返回"));
	settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
	textbox[27][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[27][0].str) / 2;
	textbox[27][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[27][0].str) / 2;
	textbox[27][0].y1 = 0.85 * SCREEN_WID ;
	textbox[27][0].y2 = 0.85 * SCREEN_WID + textheight(textbox[27][0].str);

	strcpy_T(textbox[28][0].str, _T("请选择先后手"));
	settextstyle(menuTextSize1, 0, _T("微软雅黑"));
	textbox[28][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[28][0].str) / 2;
	textbox[28][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[28][0].str) / 2;
	textbox[28][0].y1 = 0.2 * SCREEN_WID - textheight(textbox[28][0].str);
	textbox[28][0].y2 = 0.2 * SCREEN_WID;

	strcpy_T(textbox[29][0].str, _T("先手"));
	settextstyle(menuTextSize1, 0, _T("微软雅黑"));
	textbox[29][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[29][0].str) / 2;
	textbox[29][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[29][0].str) / 2;
	textbox[29][0].y1 = 0.42 * SCREEN_WID - textheight(textbox[29][0].str);
	textbox[29][0].y2 = 0.42 * SCREEN_WID;

	strcpy_T(textbox[30][0].str, _T("后手"));
	settextstyle(menuTextSize1, 0, _T("微软雅黑"));
	textbox[30][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[30][0].str) / 2;
	textbox[30][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[30][0].str) / 2;
	textbox[30][0].y1 = 0.58 * SCREEN_WID - textheight(textbox[30][0].str);
	textbox[30][0].y2 = 0.58 * SCREEN_WID;

	strcpy_T(textbox[31][0].str, _T("返回"));
	settextstyle(menuTextSize1, 0, _T("微软雅黑"));
	textbox[31][0].x1 = SCREEN_LEN / 2 - textwidth(textbox[31][0].str) / 2;
	textbox[31][0].x2 = SCREEN_LEN / 2 + textwidth(textbox[31][0].str) / 2;
	textbox[31][0].y1 = 0.75 * SCREEN_WID;
	textbox[31][0].y2 = 0.75 * SCREEN_WID + textheight(textbox[31][0].str);

	strcpy_T(textbox[32][0].str, _T("保存"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[32][0].x1 = SCREEN_LEN / 2 - 4 * textwidth(textbox[32][0].str) / 2;
	textbox[32][0].x2 = SCREEN_LEN / 2 - 2 * textwidth(textbox[32][0].str) / 2;
	textbox[32][0].y1 = 0.85 * SCREEN_WID;
	textbox[32][0].y2 = 0.85 * SCREEN_WID + textheight(textbox[32][0].str);

	strcpy_T(textbox[33][0].str, _T("返回"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[33][0].x1 = SCREEN_LEN / 2 + 2 * textwidth(textbox[33][0].str) / 2;
	textbox[33][0].x2 = SCREEN_LEN / 2 + 4 * textwidth(textbox[33][0].str) / 2;
	textbox[33][0].y1 = 0.85 * SCREEN_WID;
	textbox[33][0].y2 = 0.85 * SCREEN_WID + textheight(textbox[33][0].str);

	strcpy_T(textbox[34][0].str, _T("选择"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[34][0].x1 = 0.2 * SCREEN_LEN - textwidth(textbox[34][0].str) / 2;
	textbox[34][0].x2 = 0.2 * SCREEN_LEN + textwidth(textbox[34][0].str) / 2;
	textbox[34][0].y1 = 0.7 * SCREEN_WID;
	textbox[34][0].y2 = 0.7 * SCREEN_WID + textheight(textbox[34][0].str);

	strcpy_T(textbox[35][0].str, _T("选择"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[35][0].x1 = 0.5 * SCREEN_LEN - textwidth(textbox[35][0].str) / 2;
	textbox[35][0].x2 = 0.5 * SCREEN_LEN + textwidth(textbox[35][0].str) / 2;
	textbox[35][0].y1 = 0.7* SCREEN_WID;
	textbox[35][0].y2 = 0.7 * SCREEN_WID + textheight(textbox[35][0].str);

	strcpy_T(textbox[36][0].str, _T("选择"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[36][0].x1 = 0.8  * SCREEN_LEN - textwidth(textbox[36][0].str) / 2;
	textbox[36][0].x2 = 0.8 * SCREEN_LEN + textwidth(textbox[36][0].str) / 2;
	textbox[36][0].y1 = 0.7 * SCREEN_WID;
	textbox[36][0].y2 = 0.7 * SCREEN_WID + textheight(textbox[36][0].str);

	strcpy_T(textbox[37][0].str, _T("返回"));
	settextstyle(menuTextSize, 0, _T("微软雅黑"));
	textbox[37][0].x1 = SCREEN_LEN / 2 + 13 * textwidth(textbox[37][0].str) / 2;
	textbox[37][0].x2 = SCREEN_LEN / 2 +15* textwidth(textbox[37][0].str) / 2;
	textbox[37][0].y1 = 0.85* SCREEN_WID;
	textbox[37][0].y2 = 0.85* SCREEN_WID + textheight(textbox[37][0].str);

	for (int i = 2; i <= 6; ++i)
	{
		strcpy_T(textbox[i][1].str, textbox[i][0].str);
		settextstyle(menuTextSize1, 0, _T("微软雅黑"));
		textbox[i][1].x1 = (textbox[i][0].x1 + textbox[i][0].x2) / 2 - textwidth(textbox[i][1].str) / 2;
		textbox[i][1].x2 = textbox[i][1].x1 + textwidth(textbox[i][1].str);
		textbox[i][1].y1 = (textbox[i][0].y1 + textbox[i][0].y2) / 2 - textheight(textbox[i][1].str) / 2 - 1;
		textbox[i][1].y2 = textbox[i][1].y1 + textheight(textbox[i][1].str);
	}

	for (int i = 23; i <= 27; ++i)
	{
		strcpy_T(textbox[i][1].str, textbox[i][0].str);
		settextstyle(aiTextSizeMid, 0, _T("微软雅黑"));
		textbox[i][1].x1 = (textbox[i][0].x1 + textbox[i][0].x2) / 2 - textwidth(textbox[i][1].str) / 2;
		textbox[i][1].x2 = textbox[i][1].x1 + textwidth(textbox[i][1].str);
		textbox[i][1].y1 = (textbox[i][0].y1 + textbox[i][0].y2) / 2 - textheight(textbox[i][1].str) / 2 - 1;
		textbox[i][1].y2 = textbox[i][1].y1 + textheight(textbox[i][1].str);
	}

	for (int i = 29; i <= 31; ++i)
	{
		strcpy_T(textbox[i][1].str, textbox[i][0].str);
		settextstyle(menuTextSize2, 0, _T("微软雅黑"));
		textbox[i][1].x1 = (textbox[i][0].x1 + textbox[i][0].x2) / 2 - textwidth(textbox[i][1].str) / 2;
		textbox[i][1].x2 = textbox[i][1].x1 + textwidth(textbox[i][1].str);
		textbox[i][1].y1 = (textbox[i][0].y1 + textbox[i][0].y2) / 2 - textheight(textbox[i][1].str) / 2 - 1;
		textbox[i][1].y2 = textbox[i][1].y1 + textheight(textbox[i][1].str);
	}

	for (int i = 32; i <= 33; ++i)
	{
		strcpy_T(textbox[i][1].str, textbox[i][0].str);
		settextstyle(menuTextSize1, 0, _T("微软雅黑"));
		textbox[i][1].x1 = (textbox[i][0].x1 + textbox[i][0].x2) / 2 - textwidth(textbox[i][1].str) / 2;
		textbox[i][1].x2 = textbox[i][1].x1 + textwidth(textbox[i][1].str);
		textbox[i][1].y1 = (textbox[i][0].y1 + textbox[i][0].y2) / 2 - textheight(textbox[i][1].str) / 2 - 1;
		textbox[i][1].y2 = textbox[i][1].y1 + textheight(textbox[i][1].str);
	}

	for (int i = 34; i <= 37; ++i)
	{
		strcpy_T(textbox[i][1].str, textbox[i][0].str);
		settextstyle(menuTextSize1, 0, _T("微软雅黑"));
		textbox[i][1].x1 = (textbox[i][0].x1 + textbox[i][0].x2) / 2 - textwidth(textbox[i][1].str) / 2;
		textbox[i][1].x2 = textbox[i][1].x1 + textwidth(textbox[i][1].str);
		textbox[i][1].y1 = (textbox[i][0].y1 + textbox[i][0].y2) / 2 - textheight(textbox[i][1].str) / 2 - 1;
		textbox[i][1].y2 = textbox[i][1].y1 + textheight(textbox[i][1].str);
	}

	strcpy_T(aibox[1].str, _T("NoGo_Ramdom"));
	strcpy_T(aibox[2].str, _T("NoGo_Simple"));
	strcpy_T(aibox[3].str, _T("NoGo_Killer"));
	strcpy_T(aibox[4].str, _T("NoGo_God"));
}

void _start()
{
	cleardevice();
	clock_t st = clock();
	clock_t ed = clock();
	setbkmode(TRANSPARENT);
	do
	{
		int colorNow = color1 + 0.67*(color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(80, 0, _T("微软雅黑"));
		outtextxy(textbox[0][0].x1, textbox[0][0].y1, textbox[0][0].str);
		ed = clock();
	}while (ed - st < 1.5*CLOCKS_PER_SEC);
	Sleep(sleepTime);
	st = clock();
	ed = clock();
	do
	{
		int colorNow = color2- 0.67 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(80, 0, _T("微软雅黑"));
		outtextxy(textbox[0][0].x1, textbox[0][0].y1, textbox[0][0].str);
		ed = clock();
	} while (ed - st < 1.5 * CLOCKS_PER_SEC);
	Sleep(sleepTime);
	cleardevice();
	st = clock();
	ed = clock();
	do
	{
		int colorNow = color1 + 1* (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(menuTextSize1, 0, _T("微软雅黑"));
		outtextxy(textbox[21][0].x1, textbox[21][0].y1, textbox[21][0].str);
		ed = clock();
	} while (ed - st < 1 * CLOCKS_PER_SEC);
	Sleep(sleepTime);
	st = clock();
	ed = clock();
	do
	{
		int colorNow = color2 - 1 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC + 10;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(menuTextSize1, 0, _T("微软雅黑"));
		outtextxy(textbox[21][0].x1, textbox[21][0].y1, textbox[21][0].str);
		ed = clock();
	} while (ed - st < 1* CLOCKS_PER_SEC);
	return;
}

void _init_menu()
{
	cleardevice();
	clock_t st = clock();
	clock_t ed = clock();
	setbkmode(TRANSPARENT);
	do
	{
		int colorNow = color1 + 2 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(70, 0, _T("微软雅黑"));
		outtextxy(textbox[1][0].x1, textbox[1][0].y1, textbox[1][0].str);
		settextstyle(menuTextSize, 0, _T("微软雅黑"));
		for (int i = 2; i <= 5; ++i)
			outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
		ed = clock();
	} while (ed - st < 0.5 * CLOCKS_PER_SEC);
	return;
}

bool judge_pos(MOUSEMSG msg,text t)
{
	return (msg.x > t.x1 && msg.x<t.x2 && msg.y>t.y1 && msg.y < t.y2);
}

void _help()
{
	cleardevice();
	clock_t st = clock();
	clock_t ed = clock();
	setbkmode(TRANSPARENT);
	setlinestyle(PS_SOLID, 3);
	do
	{
		int colorNow = color1 + 2 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		setlinecolor(RGB(colorNow, colorNow, colorNow));
		rectangle(0.1 * SCREEN_LEN, 0.06 * SCREEN_WID, 0.9 * SCREEN_LEN, 0.85 * SCREEN_WID);

		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(menuTextSize, 0, _T("微软雅黑"));
		outtextxy(textbox[6][0].x1, textbox[6][0].y1, textbox[6][0].str);
		settextstyle(30, 0, _T("微软雅黑"));
		for (int i = 7; i <= 20; ++i)
			outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);

		ed = clock();
	} while (ed - st < 0.5 * CLOCKS_PER_SEC);

	bool stay = true;
	bool touch = false;
	bool touchElse = false;
	while (stay)
	{
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			FlushMouseMsgBuffer();
			if (judge_pos(msg, textbox[6][0]))
			{
				if (msg.mkLButton)
					stay = false;
				touch = true;
				touchElse = false;
				settextstyle(menuTextSize1, 0, _T("微软雅黑"));
				setfillcolor(BKCOLOR);
				solidrectangle(textbox[6][0].x1, textbox[6][0].y1, textbox[6][0].x2, textbox[6][0].y2);
				outtextxy(textbox[6][1].x1, textbox[6][1].y1, textbox[6][1].str);
			}
			else
				touchElse = true;
			if (touch && touchElse)
			{
				touch = false;
				setfillcolor(BKCOLOR);
				solidrectangle(textbox[6][1].x1, textbox[6][1].y1, textbox[6][1].x2, textbox[6][1].y2);
				settextstyle(menuTextSize, 0, _T("微软雅黑"));
				outtextxy(textbox[6][0].x1, textbox[6][0].y1, textbox[6][0].str);
			}
		}
	}
	Sleep(sleepTime);
	_init_menu();
	return;
}

int _choose_ai()
{
	cleardevice();
	clock_t st = clock();
	clock_t ed = clock();
	do
	{
		int colorNow = color1 + 2 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC ;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(aiTextSize, 0, _T("微软雅黑"));
		outtextxy(textbox[22][0].x1, textbox[22][0].y1, textbox[22][0].str);
		settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
		for(int i=23;i<=27;++i)
			outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);

		ed = clock();
	} while (ed - st < 0.5 * CLOCKS_PER_SEC);

	bool stay = true;
	bool touch[50] = {0};
	bool touchElse[50] = {0};
	while (stay)
	{
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			FlushMouseMsgBuffer();
			for (int i = 23; i <= 27; ++i)
			{
				if (judge_pos(msg, textbox[i][0]))	//返回按钮
				{
					if (msg.mkLButton)
						return i - 22;
					
					touch[i] = true;
					touchElse[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].x2, textbox[i][0].y2);
					settextstyle(aiTextSizeMid, 0, _T("微软雅黑"));
					outtextxy(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].str);
				}
				else
					touchElse[i] = true;
				if (touch[i] && touchElse[i])
				{
					touch[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].x2, textbox[i][1].y2);
					settextstyle(aiTextSizeSmall, 0, _T("微软雅黑"));
					outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
				}
			}
			
		}
	}
}

int _choose_first()
{
	cleardevice();
	clock_t st = clock();
	clock_t ed = clock();
	do
	{
		int colorNow = color1 + 2 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(menuTextSize1, 0, _T("微软雅黑"));
		for (int i = 28; i <= 31; ++i)
			outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
		ed = clock();
	} while (ed - st < 0.5 * CLOCKS_PER_SEC);

	bool stay = true;
	bool touch[50] = { 0 };
	bool touchElse[50] = { 0 };
	while (stay)
	{
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			FlushMouseMsgBuffer();
			for (int i = 29; i <= 31; ++i)
			{
				if (judge_pos(msg, textbox[i][0]))	//返回按钮
				{
					if (msg.mkLButton)
					{
						if (i == 29)
							return 1;
						if (i == 30)
							return 0;
						if (i == 31)
							return -1;
					}
					touch[i] = true;
					touchElse[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].x2, textbox[i][0].y2);
					settextstyle(menuTextSize2, 0, _T("微软雅黑"));
					outtextxy(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].str);
				}
				else
					touchElse[i] = true;
				if (touch[i] && touchElse[i])
				{
					touch[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].x2, textbox[i][1].y2);
					settextstyle(menuTextSize1, 0, _T("微软雅黑"));
					outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
				}
			}
		}
	}
}

void _init_board()
{
	cleardevice();
	myBoard.top = 0.1 * SCREEN_WID;
	myBoard.boardLen = 0.7* SCREEN_WID;
	myBoard.left = SCREEN_LEN / 2 - myBoard.boardLen / 2;
	myBoard.gridLen = myBoard.boardLen / 8;
	myBoard.right = myBoard.left + 8 * myBoard.gridLen;
	myBoard.bottom = myBoard.top + 8 * myBoard.gridLen;
	_draw_board(myBoard, board, 0.30);
	settextcolor(FTCOLOR);
	clock_t st=clock();
	clock_t ed=clock();
	do
	{
		int colorNow = color1 + 2 * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		settextstyle(menuTextSize, 0, _T("微软雅黑"));
		outtextxy(textbox[32][0].x1, textbox[32][0].y1, textbox[32][0].str);
		outtextxy(textbox[33][0].x1, textbox[33][0].y1, textbox[33][0].str);
		ed = clock();
	} while (ed - st < 0.5 * CLOCKS_PER_SEC);
	return;
}

void put_piece(int i,int j,bool piece)
{
	int posX = myBoard.left + (i-1) * myBoard.gridLen;
	int posY = myBoard.top + (j-1) * myBoard.gridLen;
	int radius = 0.42 * myBoard.gridLen;
	clock_t st = clock();
	clock_t ed = clock();
	double timeControl = 0.25;
	do
	{
		int colorPos = color1 + (1.0/ timeControl) * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		int colorNeg = color2 - colorPos+20;
		if (piece)
		{
			setlinestyle(PS_SOLID, 1);
			setlinecolor(RGB(colorPos, colorPos, colorPos));
			setfillcolor(BKCOLOR);
			fillcircle(posX, posY, radius);
		}
		else
		{
			setlinestyle(PS_SOLID, 2);
			setlinecolor(RGB(colorNeg, colorNeg, colorNeg));
			setfillcolor(RGB(colorPos, colorPos, colorPos));
			fillcircle(posX, posY, radius); 
		}
		ed = clock();
	} while (ed - st < timeControl * CLOCKS_PER_SEC);
	
}

void _draw_board(board_ myBoard,int board[11][11],double timeControl)
{
	clock_t st = clock();
	clock_t ed = clock();
	int posX;
	int posY;
	int radius = 0.42 * myBoard.gridLen;
	do
	{
		int colorNow = color1 + (1/timeControl) * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		int colorNeg = color2 - colorNow + 20;
		setlinestyle(PS_SOLID, 4);
		setlinecolor(RGB(colorNow, colorNow, colorNow));
		line(myBoard.left, myBoard.top, myBoard.right, myBoard.top);
		line(myBoard.left, myBoard.bottom, myBoard.right, myBoard.bottom);
		line(myBoard.left, myBoard.top, myBoard.left, myBoard.bottom);
		line(myBoard.right, myBoard.top, myBoard.right, myBoard.bottom);
		setlinestyle(PS_SOLID, 2);
		for (int i = 1; i <= 7; ++i)
		{
			line(myBoard.left, myBoard.top + myBoard.gridLen * i, myBoard.right, myBoard.top + myBoard.gridLen * i);
			line(myBoard.left + myBoard.gridLen * i, myBoard.top, myBoard.left + myBoard.gridLen * i, myBoard.bottom);
		}
		ed = clock();
	} while (ed - st < timeControl * CLOCKS_PER_SEC);
	st = clock();
	ed = clock();
	do
	{
		int colorNow = color1 + (1 / timeControl) * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		int colorNeg = color2 - colorNow + 20;		setlinestyle(PS_SOLID, 4);
		for (int i = 1; i <= 9; ++i)
		{
			for (int j = 1; j <= 9; ++j)
			{
				if (board[i][j] == 1)
				{
					posX = myBoard.left + (i - 1) * myBoard.gridLen;
					posY = myBoard.top + (j - 1) * myBoard.gridLen;
					setlinecolor(RGB(colorNow, colorNow, colorNow));
					setfillcolor(RGB(colorNeg, colorNeg, colorNeg));
					setlinestyle(PS_SOLID, 1);
					fillcircle(posX, posY, radius);
				}
				else if (board[i][j] == 0)
				{
					posX = myBoard.left + (i - 1) * myBoard.gridLen;
					posY = myBoard.top + (j - 1) * myBoard.gridLen;
					setfillcolor(RGB(colorNow, colorNow, colorNow));
					setlinecolor(RGB(colorNeg, colorNeg, colorNeg));
					setlinestyle(PS_SOLID, 2);
					fillcircle(posX, posY, radius);
				}
			}
		}
		ed = clock();
	} while (ed - st < timeControl * CLOCKS_PER_SEC);

	return;
}

void main_loop()
{
	bool bot_win = 0, man_win = 0;
	while (!bot_win && !man_win)
	{
		if (now_piece == bot_piece)
		{
			if (count_avail(bot_piece) == 0)
			{
				man_win = true;
				break;
			}
			if (count_avail(man_piece) == 0)
			{
				bot_win = true;
				break;
			}
			point ai_move;
			if (ai_setting == 1)
				ai_move = ai_1(bot_piece);
			else if (ai_setting == 2)
				ai_move = ai_2(bot_piece);
			else if (ai_setting == 3)
				ai_move = ai_3(bot_piece);
			else if (ai_setting == 4)
				ai_move = ai_4(bot_piece);
			board[ai_move.x][ai_move.y] = bot_piece;
			put_piece(ai_move.x, ai_move.y, bot_piece);
		}
		else
		{
			if (count_avail(bot_piece) == 0)
			{
				man_win = true;
				break;
			}
			if (count_avail(man_piece) == 0)
			{
				bot_win = true;
				break;
			}
			bool stay = true;
			bool touch[50] = { 0 };
			bool touchElse[50] = { 0 };
			int posX;
			int posY;
			int radius = 0.42 * myBoard.gridLen;
			point tmp;
			tmp.x = -1;
			while (stay)
			{
				if (MouseHit())
				{
					redraw_line(tmp);
					MOUSEMSG msg = GetMouseMsg();
					FlushMouseMsgBuffer();
					for (int i = 32; i <= 33; ++i)
					{
						if (judge_pos(msg, textbox[i][0]))	
						{
							if (msg.mkLButton)
							{
								Sleep(sleepTime);
								if (i == 32)	//存档
								{
									_init_saveload();
									int choice = choose_save();
									_init_board();
									if (choice != 3)
										write(choice);								
								}
								if (i == 33)	//返回
								{
									cleardevice();
									initialize();
									_init_menu();
									return;
								}
							}
							touch[i] = true;
							touchElse[i] = false;
							setfillcolor(BKCOLOR);
							solidrectangle(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].x2, textbox[i][0].y2);
							settextstyle(menuTextSize1, 0, _T("微软雅黑"));
							outtextxy(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].str);
						}
						else
							touchElse[i] = true;
						if (touch[i] && touchElse[i])
						{
							touch[i] = false;
							setfillcolor(BKCOLOR);
							solidrectangle(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].x2, textbox[i][1].y2);
							settextstyle(menuTextSize, 0, _T("微软雅黑"));
							outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
						}
					}
					point pos = get_mouse(msg);
					if (pos.x > 0)
					{
						if (is_legal(pos.x, pos.y, man_piece))
						{
							posX = myBoard.left + (pos.x - 1) * myBoard.gridLen;
							posY = myBoard.top + (pos.y - 1) * myBoard.gridLen;
							setlinestyle(PS_DOT, 2);
							setlinecolor(FTCOLOR);
							circle(posX, posY, radius);
							tmp.x = pos.x;
							tmp.y = pos.y;
							if (msg.mkLButton)
							{
								board[pos.x][pos.y] = man_piece;
								put_piece(pos.x, pos.y, man_piece);
								stay = false;
							}
						}
					}
				}
			}
		}
		now_piece = !now_piece;
	}
	win_lose(bot_win,man_win);

	bool stay = true;
	bool touch[50] = { 0 };
	bool touchElse[50] = { 0 };
	while (stay)
	{
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			FlushMouseMsgBuffer();
			for (int i = 32; i <= 33; ++i)
			{
				if (judge_pos(msg, textbox[i][0]))
				{
					if (msg.mkLButton)
					{
						Sleep(sleepTime);
						if (i == 32)	//存档
						{

						}
						if (i == 33)	//返回
						{
							cleardevice();
							initialize();
							_init_menu();
							return;
						}
					}
					touch[i] = true;
					touchElse[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].x2, textbox[i][0].y2);
					settextstyle(menuTextSize1, 0, _T("微软雅黑"));
					outtextxy(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].str);
				}
				else
					touchElse[i] = true;
				if (touch[i] && touchElse[i])
				{
					touch[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].x2, textbox[i][1].y2);
					settextstyle(menuTextSize, 0, _T("微软雅黑"));
					outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
				}
			}
		}
	}
}

point get_mouse(MOUSEMSG msg)
{
	point pos;
	if (msg.x > myBoard.left - myBoard.gridLen / 2 && msg.x < myBoard.right + myBoard.gridLen / 2 &&
		msg.y > myBoard.top - myBoard.gridLen / 2 && msg.y < myBoard.bottom + myBoard.gridLen / 2)
	{
		pos.x = (msg.x - myBoard.left + 0.4*myBoard.gridLen ) / myBoard.gridLen + 1;
		pos.y = (msg.y - myBoard.top + 0.4*myBoard.gridLen ) / myBoard.gridLen + 1;
	}
	else
	{
		pos.x = -1;
		pos.y = -1;
	}
	return pos;
}

void redraw_line(point tmp)
{
	int posX, posY;
	if (tmp.x > 0)
	{
		posX = myBoard.left + (tmp.x - 1) * myBoard.gridLen;
		posY = myBoard.top + (tmp.y - 1) * myBoard.gridLen;
		setfillcolor(BKCOLOR);
		solidrectangle(posX - myBoard.gridLen / 2, posY - myBoard.gridLen / 2, posX + myBoard.gridLen / 2, posY + myBoard.gridLen / 2);
		setlinecolor(FTCOLOR);
		if (tmp.x == 1 && tmp.y == 1)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX, posY, posX + myBoard.gridLen / 2, posY);
			line(posX, posY, posX, posY + myBoard.gridLen / 2);
		}
		else if (tmp.x == 1 && tmp.y == 9)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX, posY, posX + myBoard.gridLen / 2, posY);
			line(posX, posY, posX, posY - myBoard.gridLen / 2);
		}
		else if (tmp.x == 9 && tmp.y == 1)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX, posY, posX - myBoard.gridLen / 2, posY);
			line(posX, posY, posX, posY + myBoard.gridLen / 2);
		}
		else if (tmp.x == 9 && tmp.y == 9)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX, posY, posX - myBoard.gridLen / 2, posY);
			line(posX, posY, posX, posY - myBoard.gridLen / 2);
		}
		else if (tmp.x == 1 && tmp.y>1 && tmp.y<9)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX, posY - myBoard.gridLen / 2, posX, posY + myBoard.gridLen / 2);
			setlinestyle(PS_SOLID, 2);
			line(posX, posY, posX + myBoard.gridLen / 2, posY);
		}
		else if (tmp.x == 9 && tmp.y > 1 && tmp.y < 9)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX, posY - myBoard.gridLen / 2, posX, posY + myBoard.gridLen / 2);
			setlinestyle(PS_SOLID, 2);
			line(posX - myBoard.gridLen / 2, posY, posX , posY);
		}
		else if (tmp.y == 1 && tmp.x > 1 && tmp.x < 9)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX - myBoard.gridLen / 2, posY, posX + myBoard.gridLen / 2, posY);
			setlinestyle(PS_SOLID, 2);
			line(posX, posY, posX , posY+ myBoard.gridLen / 2);
		}
		else if (tmp.y == 9 && tmp.x > 1 && tmp.x < 9)
		{
			setlinestyle(PS_SOLID, 4);
			line(posX - myBoard.gridLen / 2, posY, posX + myBoard.gridLen / 2, posY);
			setlinestyle(PS_SOLID, 2);
			line(posX , posY - myBoard.gridLen / 2, posX, posY);
		}
		else if (tmp.x >1 && tmp.x < 9 && tmp.y > 1 && tmp.y < 9)
		{
			setlinestyle(PS_SOLID, 2);
			line(posX, posY - myBoard.gridLen / 2, posX, posY + myBoard.gridLen / 2);
			line(posX - myBoard.gridLen / 2, posY, posX + myBoard.gridLen / 2, posY);
		}
		tmp.x = -1;
	}
}

void win_lose(bool bot_win, bool man_win)
{	
	int textsize = 30;
	double timeControl = 0.5;
	for (int t = 0; t < 2; ++t)
	{
		clock_t st = clock();
		clock_t ed = clock();
		do
		{
			int colorNow = color1 + (1 / timeControl) * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
			settextcolor(RGB(colorNow, colorNow, colorNow));
			if (bot_win)
			{
				settextstyle(textsize, 0, _T("微软雅黑"));
				aibox[ai_setting].x1 = 0.5 * SCREEN_LEN - textwidth(aibox[ai_setting].str) / 2 - textwidth(_T("  战胜了你")) / 2;
				aibox[ai_setting].y1 = 0.005 * SCREEN_LEN;
				outtextxy(aibox[ai_setting].x1, aibox[ai_setting].y1, aibox[ai_setting].str);
				outtextxy(aibox[ai_setting].x1 + textwidth(aibox[ai_setting].str), aibox[ai_setting].y1, _T("  战胜了你"));
			}
			else if (man_win)
			{
				settextstyle(textsize, 0, _T("微软雅黑"));
				aibox[ai_setting].x1 = 0.5 * SCREEN_LEN - textwidth(aibox[ai_setting].str) / 2 + textwidth(_T("你战胜了  ")) / 2;
				aibox[ai_setting].y1 = 0.005 * SCREEN_LEN;
				outtextxy(aibox[ai_setting].x1 - textwidth(_T("你战胜了  ")), aibox[ai_setting].y1, _T("你战胜了  "));
				outtextxy(aibox[ai_setting].x1, aibox[ai_setting].y1, aibox[ai_setting].str);
			}
			ed = clock();
		} while (ed - st < timeControl * CLOCKS_PER_SEC);
		Sleep(250);
		st = clock();
		do
		{
			int colorNow = color2 + (1 / timeControl) * (color1 - color2) * (ed - st) / CLOCKS_PER_SEC;
			settextcolor(RGB(colorNow, colorNow, colorNow));
			if (bot_win)
			{
				settextstyle(textsize, 0, _T("微软雅黑"));
				aibox[ai_setting].x1 = 0.5 * SCREEN_LEN - textwidth(aibox[ai_setting].str) / 2 - textwidth(_T("  战胜了你")) / 2;
				aibox[ai_setting].y1 = 0.005 * SCREEN_LEN;
				outtextxy(aibox[ai_setting].x1, aibox[ai_setting].y1, aibox[ai_setting].str);
				outtextxy(aibox[ai_setting].x1 + textwidth(aibox[ai_setting].str), aibox[ai_setting].y1, _T("  战胜了你"));
			}
			else if (man_win)
			{
				settextstyle(textsize, 0, _T("微软雅黑"));
				aibox[ai_setting].x1 = 0.5 * SCREEN_LEN - textwidth(aibox[ai_setting].str) / 2 + textwidth(_T("你战胜了  ")) / 2;
				aibox[ai_setting].y1 = 0.005 * SCREEN_LEN;
				outtextxy(aibox[ai_setting].x1 - textwidth(_T("你战胜了  ")), aibox[ai_setting].y1, _T("你战胜了  "));
				outtextxy(aibox[ai_setting].x1, aibox[ai_setting].y1, aibox[ai_setting].str);
			}
			ed = clock();
		} while (ed - st < timeControl * CLOCKS_PER_SEC);
	}
	clock_t st = clock();
	clock_t ed = clock();
	do
	{
		int colorNow = color1 + (1 / timeControl) * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		settextcolor(RGB(colorNow, colorNow, colorNow));
		if (bot_win)
		{
			settextstyle(textsize, 0, _T("微软雅黑"));
			aibox[ai_setting].x1 = 0.5 * SCREEN_LEN - textwidth(aibox[ai_setting].str) / 2 - textwidth(_T("  战胜了你")) / 2;
			aibox[ai_setting].y1 = 0.005 * SCREEN_LEN;
			outtextxy(aibox[ai_setting].x1, aibox[ai_setting].y1, aibox[ai_setting].str);
			outtextxy(aibox[ai_setting].x1 + textwidth(aibox[ai_setting].str), aibox[ai_setting].y1, _T("  战胜了你"));
		}
		else if (man_win)
		{
			settextstyle(textsize, 0, _T("微软雅黑"));
			aibox[ai_setting].x1 = 0.5 * SCREEN_LEN - textwidth(aibox[ai_setting].str) / 2 + textwidth(_T("你战胜了  ")) / 2;
			aibox[ai_setting].y1 = 0.005 * SCREEN_LEN;
			outtextxy(aibox[ai_setting].x1 - textwidth(_T("你战胜了  ")), aibox[ai_setting].y1, _T("你战胜了  "));
			outtextxy(aibox[ai_setting].x1, aibox[ai_setting].y1, aibox[ai_setting].str);
		}
		ed = clock();
	} while (ed - st < timeControl * CLOCKS_PER_SEC);
	return;
}

void _init_saveload()
{
	cleardevice();
	ifstream file[3];
	file[0].open("./data\\1.txt",ios::in);
	file[1].open("./data\\2.txt",ios::in);
	file[2].open("./data\\3.txt",ios::in);
	for (int i = 0; i < 3; ++i)
	{
		file[i] >> saveBot[i]>>save_man_piece[i];
		saveCount[i] = 0;
		for (int j = 1; j <= 9; ++j)
		{
			for (int p = 1; p <= 9; ++p)
			{
				file[i] >> saveBoard[i][j][p];
				if (saveBoard[i][j][p] != -1)
					saveCount[i]++;
			}
		}
		file[i].close();
	}
	for (int i = 0; i < 3; ++i)
	{
		miniBoard[i].top = 0.2 * SCREEN_WID;
		miniBoard[i].boardLen = 0.2 * SCREEN_LEN;
		miniBoard[i].gridLen = miniBoard[i].boardLen / 8;
		miniBoard[i].bottom = miniBoard[i].top + miniBoard[i].boardLen;
		miniBoard[i].left = (0.1 + i * 0.3) * SCREEN_LEN;
		miniBoard[i].right = miniBoard[i].left + miniBoard[i].boardLen;
	}
	int posX;
	int posY;
	int radius = 0.42 * miniBoard[0].gridLen;
	double timeControl = 0.5;
	clock_t st = clock();
	clock_t ed = clock();
	do
	{
		int colorNow = color1 + (1 / timeControl) * (color2 - color1) * (ed - st) / CLOCKS_PER_SEC;
		int colorNeg = color2 - colorNow + 20;

		for (int i = 0; i < 3; ++i)
		{
			setlinestyle(PS_SOLID, 4);
			setlinecolor(RGB(colorNow, colorNow, colorNow));
			line(miniBoard[i].left, miniBoard[i].top, miniBoard[i].right, miniBoard[i].top);
			line(miniBoard[i].left, miniBoard[i].bottom, miniBoard[i].right, miniBoard[i].bottom);
			line(miniBoard[i].left, miniBoard[i].top, miniBoard[i].left, miniBoard[i].bottom);
			line(miniBoard[i].right, miniBoard[i].top, miniBoard[i].right, miniBoard[i].bottom);
			setlinestyle(PS_SOLID, 2);
			for (int t = 1; t <= 7; ++t)
			{
				line(miniBoard[i].left, miniBoard[i].top + miniBoard[i].gridLen * t, miniBoard[i].right, miniBoard[i].top + miniBoard[i].gridLen * t);
				line(miniBoard[i].left + miniBoard[i].gridLen * t, miniBoard[i].top, miniBoard[i].left + miniBoard[i].gridLen * t, miniBoard[i].bottom);
			}
			setlinestyle(PS_SOLID, 1);
			for (int t = 1; t <= 9; ++t)
			{
				for (int j = 1; j <= 9; ++j)
				{
					if (saveBoard[i][t][j] == 1)
					{
						posX = miniBoard[i].left + (t - 1) * miniBoard[i].gridLen;
						posY = miniBoard[i].top + (j - 1) * miniBoard[i].gridLen;
						setlinecolor(RGB(colorNow, colorNow, colorNow));
						setfillcolor(RGB(colorNeg, colorNeg, colorNeg));
						fillcircle(posX, posY, radius);
					}
					else if (saveBoard[i][t][j] == 0)
					{
						posX = miniBoard[i].left + (t - 1) * miniBoard[i].gridLen;
						posY = miniBoard[i].top + (j - 1) * miniBoard[i].gridLen;
						setfillcolor(RGB(colorNow, colorNow, colorNow));
						setlinecolor(RGB(colorNeg, colorNeg, colorNeg));
						fillcircle(posX, posY, radius);
					}
				}
			}
			settextcolor(RGB(colorNow, colorNow, colorNow));
			settextstyle(30, 0, _T("微软雅黑"));
			outtextxy((0.2 + i * 0.3) * SCREEN_LEN - 0.5 * textwidth(aibox[saveBot[i]].str), textbox[i + 34][0].y1 - 0.1 * SCREEN_WID, aibox[saveBot[i]].str);
			settextstyle(menuTextSize, 0, _T("微软雅黑"));
			outtextxy(textbox[i + 34][0].x1, textbox[i + 34][0].y1, textbox[i + 34][0].str);
			outtextxy(textbox[i + 37][0].x1, textbox[i + 37][0].y1, textbox[i + 37][0].str);
		}
		ed = clock();
	} while (ed - st < timeControl * CLOCKS_PER_SEC);
	return;
}

int choose_save()
{
	bool stay = true;
	bool touch[50] = { 0 };
	bool touchElse[50] = { 0 };
	while (stay)
	{
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			FlushMouseMsgBuffer();
			for (int i = 34; i <= 37; ++i)
			{
				if (judge_pos(msg, textbox[i][0]))	//返回按钮
				{
					if (msg.mkLButton)
						return i - 34;
						touch[i] = true;
					touchElse[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].x2, textbox[i][0].y2);
					settextstyle(menuTextSize1, 0, _T("微软雅黑"));
					outtextxy(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].str);
				}
				else
					touchElse[i] = true;
				if (touch[i] && touchElse[i])
				{
					touch[i] = false;
					setfillcolor(BKCOLOR);
					solidrectangle(textbox[i][1].x1, textbox[i][1].y1, textbox[i][1].x2, textbox[i][1].y2);
					settextstyle(menuTextSize, 0, _T("微软雅黑"));
					outtextxy(textbox[i][0].x1, textbox[i][0].y1, textbox[i][0].str);
				}
			}
		}
	}
}

void write(int choice)
{
	ofstream file;
	if (choice == 0)
		file.open("./data\\1.txt", ios::out);
	else if (choice == 1)
		file.open("./data\\2.txt", ios::out);
	else if (choice == 2)
		file.open("./data\\3.txt", ios::out);
	file.clear();
	file << ai_setting << " " << man_piece << endl;
	for (int i = 1; i <= 9; ++i)
	{
		for (int j = 1; j <= 9; ++j)
		{
			file << board[i][j] << " ";
		}
		file << endl;
	}
}


void strcpy_T(TCHAR* dst, const TCHAR* src) {
	int idx = 0;
	while (src[idx]) {
		dst[idx] = src[idx];
		++idx;
		if (idx >= 1000) break;
	}
	return;
}

// AI Function

void initialize()
{
	memset(board, -1, sizeof(board));
	for (int i = 0; i < 11; ++i)
	{
		board[0][i] = -2;
		board[10][i] = -2;
		board[i][0] = -2;
		board[i][10] = -2;
	}
	return;
}

bool inborder(int x, int y)
{
	return (x >= 1 && x <= 9 && y >= 1 && y <= 9);
}

bool search_breath(int i, int j, bool piece)	//use dfs
{
	if (board[i][j] == piece)
	{
		searchBoard[i][j] = true;
		int i1, j1;
		for (int k = 0; k < 4; ++k)
		{
			i1 = i + DIRX[k];
			j1 = j + DIRY[k];
			if (board[i1][j1] == -1)
				return true;
			else if (board[i1][j1] == piece && !searchBoard[i1][j1])
			{
				if (search_breath(i1, j1, piece))
					return true;
			}
		}
	}
	return false;
}

bool is_legal(int i, int j, bool move)
{

	if (board[i][j] != -1)
		return false;
	else
	{
		board[i][j] = move;
		memset(searchBoard, 0, sizeof(searchBoard));
		if (!search_breath(i, j, move))
		{
			board[i][j] = -1;
			return false;
		}

		bool op = !move;
		int i1, j1;
		for (int k = 0; k < 4; ++k)
		{
			i1 = i + DIRX[k];
			j1 = j + DIRY[k];
			if (board[i1][j1] == op)
			{
				memset(searchBoard, 0, sizeof(searchBoard));
				if (!search_breath(i1, j1, op))	//judge if this move eats op's piece
				{
					board[i][j] = -1;
					return false;
				}
			}
		}
		board[i][j] = -1;	//restore the board
	}
	return true;
}

int count_avail(bool piece)		
{
	int count = 0;
	for (int i = 1; i <= 9; ++i)
		for (int j = 1; j <= 9; ++j)
			if (is_legal(i, j, piece))
				count++;
	return count;
}

inline int val_state(bool piece)
{
	return count_avail(piece) - count_avail(!piece);
}

int max_val(bool piece, int layer)
{
	if (count_avail(piece) == 0) return 0;
	if (layer <= 0)
		return val_state(piece);
	int tmp;
	int max1 = -100;
	for (int i = 1; i <= 9; ++i)
	{
		for (int j = 1; j <= 9; ++j)
		{
			if (is_legal(i, j, piece))
			{
				board[i][j] = piece;
				tmp = -max_val(!piece, --layer); //evaluation
				board[i][j] = -1;
				if (tmp > max1)	max1 = tmp;
			}

		}
	}
	return max1;
}

int minimax(int i, int j, bool piece, int layer)
//compute the value of (i,j) for the piece side
{
	board[i][j] = piece;
	int val = -max_val(!piece, layer);
	board[i][j] = -1;
	return val;
}

bool rule(point p1, point p2)
{
	return p1.val > p2.val;
}

double MCTS(int layer, double timelimit, bool bot_piece)
{
	int tmp_board[11][11];
	point legalPoint[100];
	int size;
	point tmp_p;

	int state, val = 0;
	int searchToEnd = -1;
	int layer1;
	int testnum = 0;

	clock_t timeStart;
	clock_t timeNow;
	memcpy(tmp_board, board, sizeof(board));
	timeStart = clock();

	do
	{
		//初始化
		searchToEnd = -1;
		layer1 = layer;
		memcpy(board, tmp_board, sizeof(board));

		while (layer1--)
		{
			size = 0;
			for (int i = 1; i <= 9; ++i)
			{
				for (int j = 1; j <= 9; ++j)
				{
					if (is_legal(i, j, !bot_piece))
					{
						legalPoint[size].x = i;
						legalPoint[size].y = j;
						++size;
					}
				}
			}
			if (!size)
			{
				searchToEnd = 0;
				break;
			}

			tmp_p = legalPoint[rand() % size];
			board[tmp_p.x][tmp_p.y] = !bot_piece; 	//对手完成落子

			size = 0;
			for (int i = 1; i <= 9; ++i)
			{
				for (int j = 1; j <= 9; ++j)
				{
					if (is_legal(i, j, bot_piece))
					{
						legalPoint[size].x = i;
						legalPoint[size].y = j;
						++size;
					}
				}
			}
			if (!size)
			{
				searchToEnd = 1;
				break;
			}
			tmp_p = legalPoint[rand() % size];
			board[tmp_p.x][tmp_p.y] = bot_piece; 	//自己完成落子
		}
		state = val_state(bot_piece);
		val += state;
		if (searchToEnd == 0) val += 2;
		else if (searchToEnd == 1) val -= 2;
		timeNow = clock();
		++testnum;
	} while (timeNow - timeStart < timelimit);

	memcpy(board, tmp_board, sizeof(board));
	test += testnum;
	return	(1.0 * val) / testnum;
}

point ai_1(bool piece)
{
	global_avail = count_avail(piece);
	point pSet[100];
	int size_pSet = 0;
	for (int i = 1; i <= 9; ++i)
	{
		for (int j = 1; j <= 9; ++j)
		{
			if (is_legal(i, j, piece))
			{
				pSet[size_pSet].x = i;
				pSet[size_pSet].y = j;
				++size_pSet;
			}
		}
	}
	return pSet[rand() % size_pSet];
}

point ai_2(bool piece)
{
	global_avail = count_avail(piece);
	int layer1 = 0;
	point pSet[100];
	int size_pSet = 0;
	for (int i = 1; i <= 9; ++i)
	{
		for (int j = 1; j <= 9; ++j)
		{
			if (is_legal(i, j, piece))
			{
				pSet[size_pSet].x = i;
				pSet[size_pSet].y = j;
				pSet[size_pSet].val = minimax(i, j, piece, layer1);
				++size_pSet;
			}
		}
	}
	sort(pSet, pSet + size_pSet, rule);
	int sumMax = 0;
	int maxVal = pSet[0].val;
	for (int it = 0; it < size_pSet; ++it)
	{
		if (pSet[it].val == maxVal) sumMax++;
	}
	return pSet[rand() % sumMax];
}

point ai_3(bool piece)
{
	global_avail = count_avail(piece);
	int layer1 = 4;
	point pSet[100];
	int size_pSet = 0;
	for (int i = 1; i <= 9; ++i)
	{
		for (int j = 1; j <= 9; ++j)
		{
			if (is_legal(i, j, piece))
			{
				pSet[size_pSet].x = i;
				pSet[size_pSet].y = j;
				pSet[size_pSet].val = minimax(i, j, piece, layer1);
				++size_pSet;
			}
		}
	}
	sort(pSet, pSet + size_pSet, rule);
	int sumMax = 0;
	int maxVal = pSet[0].val;
	for (int it = 0; it < size_pSet; ++it)
	{
		if (pSet[it].val == maxVal) sumMax++;
	}
	return pSet[rand() % sumMax];
}

point ai_4(bool piece)
{
	global_avail = count_avail(piece);
	int layer1 = 2;
	point pSet[100];
	int size_pSet = 0;
	for (int i = 1; i <= 9; ++i)
	{
		for (int j = 1; j <= 9; ++j)
		{
			if (is_legal(i, j, piece))
			{
				pSet[size_pSet].x = i;
				pSet[size_pSet].y = j;
				pSet[size_pSet].val = minimax(i, j, piece, layer1);
				++size_pSet;
			}
		}
	}
	sort(pSet, pSet + size_pSet, rule);
	int sumMax = 0;
	int maxVal = pSet[0].val;
	for (int it = 0; it < size_pSet; ++it)
	{
		if (pSet[it].val == maxVal) sumMax++;
	}
	if (sumMax == 1) return pSet[0];
	int tmp;
	point p;
	clock_t timeNow = clock();
	int timeLast = CLOCKS_PER_SEC ;

	double timeLimit = 0.95 * timeLast / sumMax;

	int layer;
	if (global_avail < 30) layer = 5;
	else if (global_avail < 50) layer = 6;
	else layer = 7;

	int i1, j1;
	double max_val = -10000;
	double tmp_val;
	for (int it = 0; it < sumMax; ++it)
	{
		i1 = pSet[it].x;
		j1 = pSet[it].y;
		board[i1][j1] = piece;
		tmp_val = MCTS(layer, timeLimit, piece);
		if (tmp_val > max_val)
		{
			max_val = tmp_val;
			p.x = i1;
			p.y = j1;
		}
		board[i1][j1] = -1;
	}

	return p;
}
