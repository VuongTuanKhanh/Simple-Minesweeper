// nho chinh man hinh console dung kich thuoc
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<windows.h>
#include<time.h>
#define CONSOLE_WIDTH 123		// kich thuoc man hinh console
#define CONSOLE_HEIGHT 39
#define SIZE 100
#define CURSOR 26
#define MINE 9
#define FLAG 6
#define YELLOW 14
#define BLACK 0
#define BLUE 9
#define WHITE 11

enum State {flagged,notflagged,opened};

struct GameBoard
{
	int value;
	State status;
};
struct	Level
{
	int row,col;
	int mine;
} LV[3] = {{9,9,10},{16,16,40},{16,30,99}};	//setup level

void showMainMenu();
void showLevelMenu();
int catchEventMenu(int xCur, int yTop, int yBot);
void drawMenu(const char TITLE[], const char ITEM[][SIZE], const int nITEM, int &xCur, int &yTop, int &yBot);
void drawDelCur(int x, int y, bool k);		// true/false = draw/delete
void gamePlay(Level LV);
int catchEventGameplay(Level LV, COORD coorFirstCell);
void openCell(GameBoard CELL[][SIZE], COORD pos, COORD coorFirstCell, int &countOpenCell);
void openMinefield(GameBoard CELL[][SIZE], Level LV, COORD coorFirstCell, int &countOpenCell, bool checkWin);
void printCell(int character, COORD coorPrint);
void markCell(GameBoard CELL[][SIZE], COORD pos, bool checkMark);
void setupMinefield(GameBoard CELL[][SIZE], Level LV, COORD posStart);
void randomMinefield(GameBoard CELL[][SIZE], Level LV, COORD posStart);
void drawChessBoard(int row, int col, COORD &coorFirstCell);
void drawDetail(COORD coorFirstCell, Level LV, COORD &coorRemainMine);
void updateRemainMine(int &remainMine, int change, COORD coorRemainMine);
void showEndGame(char result[]);
void drawEndGame(char result[]);
void showTutorial();
int random(int a, int b);
void gotoxy(int x,int y);
int whereX();
int whereY();
void showCursor(bool CursorVisibility, int sizeCursor=100);
void textColor(SHORT color);
void bgColor(SHORT color);
void resetColor();

const char TITLE[] = "MINESWEEPER";
const int X[9] = {-1,-1,-1,0,1,1,1,0,0};
const int Y[9] = {-1,0,1,1,1,0,-1,-1,0};

void main()
{
	srand((unsigned)time(NULL));
	resetColor();
	showMainMenu();
}
void showMainMenu()
{
	system("cls");
	showCursor(false);		// hide cursor
	const char ITEM_MENU[][SIZE] = {"Play",
									"Tutorial",
									"Exit"};
	const int nITEM_MENU = sizeof(ITEM_MENU)/(sizeof(char)*SIZE);
	int xCur,yTop,yBot;
	drawMenu(TITLE,ITEM_MENU,nITEM_MENU,xCur,yTop,yBot);
	while (true)
	{
		Sleep(200);
		int key = catchEventMenu(xCur,yTop,yBot);
		switch (key)
		{
			case 1:		//Play
				showLevelMenu();
			case 2:		//Tutorial
				showTutorial();
			case 3:		//Exit
				exit(0);
		}
	}
}
void showLevelMenu()
{
	system("cls");
	showCursor(false);
	const char ITEM_LEVEL[][SIZE] = {"Easy (9x9: 10 mines)",
									 "Normal (16x16: 40 mines)",
									 "Hard (16x30: 99 mines)",
									 "Back"};
	const int nITEM_LEVEL = sizeof(ITEM_LEVEL)/(sizeof(char)*SIZE);
	int xCur,yTop,yBot;
	drawMenu(TITLE,ITEM_LEVEL,nITEM_LEVEL,xCur,yTop,yBot);
	while (true)
	{
		Sleep(200);
		int key = catchEventMenu(xCur,yTop,yBot);		// return 1,2,3 or 4: when press ENTER key
		switch (key)
		{
			case 1:		//Easy
				gamePlay(LV[0]);
			case 2:		//Normal
				gamePlay(LV[1]);
			case 3:		//Hard
				gamePlay(LV[2]);
			case 4:		//Back
				showMainMenu();
		}
	}
}
int catchEventMenu(int xCur, int yTop, int yBot)
{
	if (GetAsyncKeyState(VK_UP) && whereY()!=yTop)		//check UP key && check top border
	{
		// move up
		drawDelCur(xCur,whereY(),false);
		drawDelCur(xCur,whereY()-2,true);
	}
	else if (GetAsyncKeyState(VK_DOWN) && whereY()!=yBot)//check DOWN key && check bot border
	{
		//move down
		drawDelCur(xCur,whereY(),false);
		drawDelCur(xCur,whereY()+2,true);
	}
	else if (GetAsyncKeyState(VK_RETURN))//check ENTER key
		return (whereY()-yTop)/2+1;		//return value equivalent
	return 0;
}
void drawMenu(const char TITLE[], const char ITEM[][SIZE], const int nITEM, int &xCur, int &yTop, int &yBot)
{
	textColor(YELLOW);
	int y = 5;		//y coor title
	gotoxy((CONSOLE_WIDTH-strlen(TITLE))/2,y);
	puts(TITLE);
	y += 4;
	yTop = y;		// y coor first item			
	int x = (CONSOLE_WIDTH-strlen(ITEM[0]))/2;
	for (int i=0;i<nITEM;i++)
	{
		gotoxy(x,y);
		yBot = y;	// y coor last item
		y += 2;
		puts(ITEM[i]);
	}
	xCur = x-2;		// x coor cursor
	drawDelCur(xCur,yTop,true);
	resetColor();	//return default color
}
void drawDelCur(int x, int y, bool k)
{
	textColor(YELLOW);
	gotoxy(x,y);
	if (k)
		printf("%c",CURSOR);	//draw cursor
	else
		printf(" ");			//delete cursor
	resetColor();
}
void gamePlay(Level LV)
{
	system("cls");
	COORD coorFirstCell; //coor first cell
	drawChessBoard(LV.row,LV.col,coorFirstCell);
	COORD coorRemainMine;
	drawDetail(coorFirstCell,LV,coorRemainMine);
	showCursor(true);
	GameBoard CELL[SIZE][SIZE];	// gameboard
	bool start = true;			// check start
	int countOpenCell = 0;
	int remainMine = LV.mine;
	while (true)
	{
		Sleep(200);
		int key = catchEventGameplay(LV,coorFirstCell);	// return 1: ENTER, 2: SPACE
		COORD pos = {(whereY()-coorFirstCell.Y)/2+1,(whereX()-coorFirstCell.X)/4+1};	//pos of current cell {row,col}
		if (key==1)		// enter
		{
			if (start)
			{
				setupMinefield(CELL,LV,pos);
				start = false;
			}
			if (CELL[pos.X][pos.Y].status==notflagged)	// check status: not flagged yet
			{
				openCell(CELL,pos,coorFirstCell,countOpenCell);		// open cell
				if (CELL[pos.X][pos.Y].value==MINE)					// check lose
				{
					openMinefield(CELL,LV,coorFirstCell,countOpenCell,false);
					showEndGame("YOU LOSE");
				}
				else if (countOpenCell==LV.col*LV.row-LV.mine)		//check win
				{
					openMinefield(CELL,LV,coorFirstCell,countOpenCell,true);
					showEndGame("YOU WIN");
				}
			}
		}
		else if (key==2)	// space
		{
			if (CELL[pos.X][pos.Y].status==notflagged)
			{
				markCell(CELL,pos,true);		// mark
				updateRemainMine(remainMine,-1,coorRemainMine);
			}
			else if (CELL[pos.X][pos.Y].status==flagged)
			{
				markCell(CELL,pos,false);	//unmark
				updateRemainMine(remainMine,1,coorRemainMine);
			}
		}
	}
}
int catchEventGameplay(Level LV, COORD coorFirstCell)
{
	// find border
	int yTop  = coorFirstCell.Y;
	int yBot  = coorFirstCell.Y+2*(LV.row-1);
	int xLeft = coorFirstCell.X;
	int xRight= coorFirstCell.X+4*(LV.col-1);
	//catch and handling event
	if (GetAsyncKeyState(VK_UP) && whereY()!=yTop)
		gotoxy(whereX(),whereY()-2);							//move up
	else if (GetAsyncKeyState(VK_DOWN) && whereY()!=yBot)		
		gotoxy(whereX(),whereY()+2);							//move down
	else if (GetAsyncKeyState(VK_LEFT) && whereX()!=xLeft)
		gotoxy(whereX()-4,whereY());							//move left
	else if (GetAsyncKeyState(VK_RIGHT) && whereX()!=xRight)
		gotoxy(whereX()+4,whereY());							//move right
	else if (GetAsyncKeyState(VK_RETURN))
		return 1;												//return 1 when press ENTER
	else if (GetAsyncKeyState(VK_SPACE))
		return 2;												//return 2 when press SPACE
	return 0;
}
void openCell(GameBoard CELL[][SIZE], COORD pos, COORD coorFirstCell, int &countOpenCell)
{
	COORD currentCoor = {whereX(),whereY()};
	COORD coorPrint = {coorFirstCell.X-1+4*(pos.Y-1),coorFirstCell.Y+2*(pos.X-1)};
	printCell(CELL[pos.X][pos.Y].value,coorPrint);
	countOpenCell++;		// count cell opened
	CELL[pos.X][pos.Y].status = opened;
	if (CELL[pos.X][pos.Y].value==0)
	{
		for (int i=0;i<8;i++)
		{
			COORD newPos = {pos.X+X[i],pos.Y+Y[i]};		// pos neighbor cell
			if (CELL[newPos.X][newPos.Y].status==notflagged)
				openCell(CELL,newPos,coorFirstCell,countOpenCell);
		}
	}
	gotoxy(currentCoor.X,currentCoor.Y);		// return coor start open
}
void openMinefield(GameBoard CELL[][SIZE], Level LV, COORD coorFirstCell, int &countOpenCell, bool checkWin)
{
	for (int i=1;i<=LV.row;i++)
		for (int j=1;j<=LV.col;j++)
			if (checkWin==true || (checkWin==false && CELL[i][j].value==MINE))
			{
				COORD pos = {i,j};
				openCell(CELL,pos,coorFirstCell,countOpenCell);
			}
}
void printCell(int value, COORD coorPrint)
{
	gotoxy(coorPrint.X,coorPrint.Y);
	if (value==0)
		value = 32;   // space
	else if (value==MINE)
		value = 2;	 // icon mine
	else
		value += '0';// char number
	textColor(WHITE);
	printf("%c",222);
	bgColor(WHITE);
	if (value==2)
		textColor(BLACK);
	else
		textColor(BLUE);
	printf("%c",value);
	bgColor(BLACK);
	textColor(WHITE);
	printf("%c",221);
	resetColor();		//return default color
}
void markCell(GameBoard CELL[][SIZE], COORD pos, bool checkMark)
{
	textColor(YELLOW);
	if (checkMark)
	{
		printf("%c",FLAG);
		CELL[pos.X][pos.Y].status = flagged;
	}
	else
	{
		printf(" ");
		CELL[pos.X][pos.Y].status = notflagged;
	}
	gotoxy(whereX()-1,whereY());
	resetColor();
}
void setupMinefield(GameBoard CELL[][SIZE], Level LV, COORD posStart)
{
	randomMinefield(CELL,LV,posStart);
	for (int i=1;i<=LV.row;i++)
		for (int j=1;j<=LV.col;j++)
			if (CELL[i][j].value!=MINE)
			{
				int countMine = 0;
				for (int k=0;k<8;k++)
					if (CELL[i+X[k]][j+Y[k]].value==MINE)
						countMine++;
				CELL[i][j].value = countMine;
			}
}
void randomMinefield(GameBoard CELL[][SIZE], Level LV, COORD posStart)
{
	int i,j;
	//initialize Gameboard
	for (i=0;i<=LV.row+1;i++)
		for (j=0;j<=LV.col+1;j++)
		{
			CELL[i][j].value = 0;
			if (i==0 || i==LV.row+1 || j==0 || j==LV.col+1)
				CELL[i][j].status = opened;
			else
				CELL[i][j].status= notflagged;
		}
	// create random mine
	int countMine = 0;
	while (countMine<LV.mine)
	{
		do
		{
			randomAgain:
			i = random(1,LV.row);
			j = random(1,LV.col);
			for (int k=0;k<9;k++)
				if (i==posStart.X+X[k] && j==posStart.Y+Y[k])		//posStart.X is row, posStart.Y is col
					goto randomAgain;
		} while (CELL[i][j].value==MINE);
		CELL[i][j].value = MINE;
		countMine++;
	}
}
void showEndGame(char result[])
{
	drawEndGame(result);
	Sleep(200);
	while (true)
		if (GetAsyncKeyState(VK_RETURN))		// check ENTER key
			break;
	showMainMenu();
}
void drawEndGame(char result[])
{
	showCursor(false);
	char TIP[] = "Press ENTER to back menu";
	bgColor(YELLOW);
	textColor(BLACK);
	gotoxy((CONSOLE_WIDTH-strlen(result))/2,CONSOLE_HEIGHT/2-1);
	puts(result);
	gotoxy((CONSOLE_WIDTH-strlen(TIP))/2,CONSOLE_HEIGHT/2);
	puts(TIP);
	resetColor();
}
void drawChessBoard(int row, int col, COORD &coorFirstCell)
{
	textColor(YELLOW);
	int x = (CONSOLE_WIDTH-(4*col+1))/2;			// coor draw
	int y = (CONSOLE_HEIGHT-2-(2*row+1))/2;
	coorFirstCell.X = x+2;			// coor first cell
	coorFirstCell.Y = y+1;
	//draw top line
	int i,j;
	gotoxy(x,y++);
	printf("%c%c%c%c",218,196,196,196);
	for (i=0;i<col-1;i++)
		printf("%c%c%c%c",194,196,196,196);
	printf("%c",191);
	//draw mid line
	for (i=0;i<row-1;i++)
	{
		gotoxy(x,y++);
		for (j=0;j<col;j++)
			printf("%c%c%c%c",179,32,32,32);
		printf("%c",179);
		gotoxy(x,y++);
		printf("%c%c%c%c",195,196,196,196);
		for (j=0;j<col-1;j++)
			printf("%c%c%c%c",197,196,196,196);
		printf("%c",180);
	}

	gotoxy(x,y++);
	for (j=0;j<col;j++)
			printf("%c%c%c%c",179,32,32,32);
	printf("%c",179);
	// draw bot line
	gotoxy(x,y++);
	printf("%c%c%c%c",192,196,196,196);
	for (i=0;i<col-1;i++)
		printf("%c%c%c%c",193,196,196,196);
	printf("%c",217);
	gotoxy(coorFirstCell.X,coorFirstCell.Y);  // return cursor to first cell
	resetColor();
}
void drawDetail(COORD coorFirstCell, Level LV, COORD &coorRemainMine)
{
	textColor(YELLOW);
	gotoxy((CONSOLE_WIDTH-strlen(TITLE))/2,coorFirstCell.Y-3);
	puts(TITLE);
	gotoxy(coorFirstCell.X-1,coorFirstCell.Y-2);
	printf("Minefield: %dx%d (%d mines)",LV.row,LV.col,LV.mine);
	gotoxy(coorFirstCell.X-1,coorFirstCell.Y+2*LV.row);
	printf("Mines: ",LV.mine);
	coorRemainMine.X = whereX();
	coorRemainMine.Y = whereY();
	printf("%d",LV.mine);
	gotoxy(coorFirstCell.X,coorFirstCell.Y);
	resetColor();
}
void updateRemainMine(int &remainMine, int change, COORD coorRemainMine)
{
	COORD coor = {whereX(),whereY()};
	textColor(YELLOW);
	remainMine += change;
	gotoxy(coorRemainMine.X,coorRemainMine.Y);
	printf("%d\t",remainMine);
	resetColor();
	gotoxy(coor.X,coor.Y);
}
void showTutorial()
{
	system("cls");
	showCursor(false);
	const char TITLE[] = "TUTORIAL";
	const char LINE[][SIZE] = {"ARROW keys is used to MOVE between the cells",
							   "ENTER key is used to OPEN cells",
							   "SPACE key is used to MARK or UNMARK cells"};
	const int nLine = sizeof(LINE)/(sizeof(char)*SIZE);
	int xCur,yTop,yBot;
	drawMenu(TITLE,LINE,nLine,xCur,yTop,yBot);
	gotoxy(xCur,yTop);
	printf(" ");
	textColor(YELLOW);
	const char TIP[] = "Press ESC to back menu";
	gotoxy((CONSOLE_WIDTH-strlen(TIP))/2,yBot+4);
	puts(TIP);
	while (true)
		if (GetAsyncKeyState(VK_ESCAPE))
			showMainMenu();
}
int random(int a, int b)
{
	return a+rand()%(b-a+1);
}
void gotoxy(int x,int y)
{
     HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
     COORD position = {x,y};
     SetConsoleCursorPosition(hStdout,position);
}
int whereX()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.X;
}
int whereY()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.Y;
}
void showCursor(bool CursorVisibility, int sizeCursor)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor = {sizeCursor,CursorVisibility};
	SetConsoleCursorInfo(handle,&cursor);
}
void textColor(SHORT color)
{
	CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdout, &ConsoleInfo);
	WORD wAttributes = ConsoleInfo.wAttributes;
	color &= 0x000f;
	wAttributes &= 0xfff0; // Cai 0 cho 4 bit cuoi
	wAttributes |= color;
	SetConsoleTextAttribute(hStdout, wAttributes);
}
void bgColor(SHORT color)
{
	CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdout, &ConsoleInfo);
	WORD wAttributes = ConsoleInfo.wAttributes;
	color &= 0x000f;
	color <<= 4; // Dich trai 3 bit de phu hop voi mau nen
	wAttributes &= 0xff0f; // Cai 0 cho 1 bit chu nhay va 3 bit mau nen
	wAttributes |= color;
	SetConsoleTextAttribute(hStdout, wAttributes);
}
void resetColor()
{
	bgColor(0);
	textColor(7);
}