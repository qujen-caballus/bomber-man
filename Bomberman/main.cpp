#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include<time.h>
#define MAP_WIDTH 31
#define MAP_HEIGHT 30
//âÊñ Ç…âfÇÈÉTÉCÉY
#define SCREEN_WIDTH 20

enum {
	DIRECTION_NORTH,
	DIRECTION_WEST,
	DIRECTION_SOUTH,
	DIRECTION_EAST,
	DIRECTION_MAX
};

int directions[][2] = {
	{0 , -1}	,//	DIRECTION_NORTH
	{-1,0}	,//	DIRECTION_WEST
	{0 , 1}	,//	DIRECTION_SOUTH
	{1 , 0}	//	DIRECTION_EAST
};
int exitX, exitY;

enum {
	CELL_TYPE_NONE,
	CELL_TYPE_HARD_BLOCK,
	CELL_TYPE_SOFT_BLOCK,
	CELL_TYPE_EXPLOSION,
	CELL_TYPE_EXIT,
	CELL_TYPE_MAX
};

int cells[MAP_HEIGHT][MAP_WIDTH];
char cellAA[][2 + 1] = {
	"Å@",
	"Å°",
	"Å†",
	"Å¶",
	"Ex"
};

typedef struct {
	int x, y;
	int direction;
	bool isDead;
}MONSTER;

#define MONSTERS_MAX 9
MONSTER monsters[MONSTERS_MAX];

typedef struct {
	int x, y;
	int count;
}BOMB;

#define BOMB_MAX 10
#define BOMB_COUNT_MAX 10

BOMB bombs[BOMB_MAX];

int getMonster(int _x, int _y) {
	for (int i = 0; i < MONSTERS_MAX; i++)
		if ((!monsters[i].isDead) && (monsters[i].x == _x) && (monsters[i].y == _y))
			return i;
	return -1;
}

void setFreePosition(int* _pX, int* _pY) {
	while (1) {
		int x = rand() % MAP_WIDTH;
		int y = rand() % MAP_HEIGHT;
		switch (cells[y][x])
		{
		case CELL_TYPE_HARD_BLOCK:
		case CELL_TYPE_SOFT_BLOCK:
			break;
		default:
		{
			int monster = getMonster(x, y);
			if (monster < 0) {
				*_pX = x;
				*_pY = y;
				return;
			}
		}
		break;
		}
	}
}

void setRandomSoftBlockPosition(int *_pX, int *_pY) {
	while (1) {
		int x = rand() % MAP_WIDTH;
		int y = rand() % MAP_HEIGHT;
		if (cells[y][x] == CELL_TYPE_SOFT_BLOCK) {
			*_pX = x;
			*_pY = y;
			return;
		}
	}
}

int getFreeBomb() {
	for (int i = 0; i < BOMB_MAX; i++)
		if (bombs[i].count <= 0)
			return i;
	return -1;
}

int getBomb(int _x, int _y)
{
	for (int i = 0; i < BOMB_MAX; i++)
		if ((bombs[i].count > 0) && (bombs[i].x == _x) && (bombs[i].y == _y))
			return i;
	return -1;
}

bool checkCanMove(int _x, int _y) {
	switch (cells[_y][_x]) {
	case CELL_TYPE_HARD_BLOCK:
	case CELL_TYPE_SOFT_BLOCK:
		return false;
	default:
		if ((getMonster(_x, _y) < 0) && (getBomb(_x, _y) < 0))
			return true;

	}
	return false;
}
void display() {
	system("cls");
	int left = monsters[0].x - SCREEN_WIDTH / 2;
	if (left < 0)
		left = 0;
	if (left > MAP_WIDTH - SCREEN_WIDTH)
		left = MAP_WIDTH - SCREEN_WIDTH;
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = left; x < left + SCREEN_WIDTH; x++) {
			int monster = getMonster(x, y);
			int bomb = getBomb(x, y);
			if (monster > 0)
				printf("Åú");
			else if (monster == 0)
				printf("Åù");
			else if (bomb >= 0)
			{
				char aa[] = "ÇO";
				aa[1] += bombs[bomb].count;
				printf(aa);
			}
			else
				printf(cellAA[cells[y][x]]);
		}
		printf("\n");
	}
}

void gameOver() {
	monsters[0].isDead = true;
	display();
	printf("GAME OVER\a");
	_getch();
	exit(0);
}

void explosion(int _x, int _y) {
	cells[_y][_x] = CELL_TYPE_EXPLOSION;
	for (int j = 0; j < DIRECTION_MAX; j++) {
		int x = _x;
		int y = _y;
		for (int i = 0; i < 2; i++) {
			x += directions[j][0];
			y += directions[j][1];
			if (cells[y][x] == CELL_TYPE_HARD_BLOCK)
				break;
			else if (cells[y][x] == CELL_TYPE_SOFT_BLOCK) {
				cells[y][x] = CELL_TYPE_EXPLOSION;
				break;
			}
			else {
				int monster = getMonster(x, y);
				if (monster > 0) {
					monsters[monster].isDead = true;
				}
				else if (monster == 0) {
					cells[y][x] = CELL_TYPE_EXPLOSION;
					gameOver();
				}

				int bomb = getBomb(x, y);
				if (bomb >= 0) {
					bombs[bomb].count = 0;
					explosion(x, y);
				}
				cells[y][x] = CELL_TYPE_EXPLOSION;
			}
		}
	}

}


int main(){
	srand((unsigned int)time(NULL));
	
	for (int y = 0; y < MAP_HEIGHT; y += MAP_HEIGHT - 1) {
		for (int x = 0; x < MAP_WIDTH; x++)
			cells[y][x] = CELL_TYPE_HARD_BLOCK;
	}

	for (int y = 0; y < MAP_HEIGHT; y++)
		for (int x = 0; x < MAP_WIDTH; x += MAP_WIDTH - 1)
			cells[y][x] = CELL_TYPE_HARD_BLOCK;

	for (int y = 1; y < MAP_HEIGHT - 1; y++)
		for (int x = 1; x < MAP_WIDTH - 1; x++) {
			if ((x % 2 == 0) && (y % 2 == 0))
				cells[y][x] = CELL_TYPE_HARD_BLOCK;
			else if (rand() % 2)
				cells[y][x] = CELL_TYPE_SOFT_BLOCK;
		}
	cells[1][1] = cells[2][1] = cells[1][2] = CELL_TYPE_NONE;

	setRandomSoftBlockPosition(&exitX, &exitY);

	monsters[0].x =
		monsters[0].y = 1;

	for (int i = 1; i < MONSTERS_MAX; i++) {
		setFreePosition(&monsters[i].x, &monsters[i].y);
	}

	while (1)
	{
		display();
		for (int y = 1; y < MAP_HEIGHT - 1; y++)
			for (int x = 1; x < MAP_WIDTH - 1; x++)
				if (cells[y][x] == CELL_TYPE_EXPLOSION) {
					cells[y][x]  = ((x == exitX) &&(y == exitY)) ?
						CELL_TYPE_EXIT : CELL_TYPE_NONE;
				}
		int x = monsters[0].x;
		int y = monsters[0].y;
		switch (_getch()) {
		case'w': y--; break;
		case's': y++; break;
		case'a': x--;  break;
		case'd': x++; break;
		case ' ':
		{
			int bomb = getFreeBomb();
			if (bomb >= 0) {
				bombs[bomb].x = monsters[0].x;
				bombs[bomb].y = monsters[0].y;
				bombs[bomb].count = BOMB_COUNT_MAX;
			}
		}break;
		}

		if (getMonster(x, y) > 0) {
			gameOver();
		}

		if (checkCanMove(x, y)) {
		
			monsters[0].x = x;
			monsters[0].y = y;

			if (cells[y][x] == CELL_TYPE_EXIT) {
				int enemyCount = 0;
				for (int i = 1; i < MONSTERS_MAX; i++) {
					if (!monsters[i].isDead) {
						enemyCount++;
						break;
					}
				}
				if (enemyCount <= 0) {
					printf("CLEAR!!\a");
					_getch();
					exit(0);
				}
			}
		}


		for (int i = 0; i < BOMB_MAX; i++) {
			if (bombs[i].count <= 0)
				continue;
			bombs[i].count--;
			if (bombs[i].count <= 0)
			{
				explosion(bombs[i].x, bombs[i].y);
							}
			
		}

		for (int i = 1; i < MONSTERS_MAX; i++) {
			if (monsters[i].isDead)
				continue;
			int x = monsters[i].x + directions[monsters[i].direction][0];
			int y = monsters[i].y + directions[monsters[i].direction][1];
			if (getMonster(x, y) == 0) {
				monsters[i].x = x;
				monsters[i].y = y;
				gameOver();
			}
			else if (checkCanMove(x, y)) {
				monsters[i].x = x;
				monsters[i].y = y;
			}
			else
				monsters[i].direction = rand() % DIRECTION_MAX;
		}
	}
}