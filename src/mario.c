#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>


#define P '@' // player
#define L '#' // platform
#define N '.' // no space
#define E ' ' // empty space
#define X 40 // width
#define Y 20 // hight


char gameworld[Y + 1][X + 1];

int px = 1,py = Y - 1;

int jump_h = 8;

int score = 0;

int is_jumping = 0;

int jump = 0;

void print_game()
{
	for(int i = 0;i <= Y;i++)
	{
		for(int j = 0;j <= X;j++)
		{
			if(py == i && px == j) gameworld[i][j] = P;
			else if(0 == i || 0 == j) gameworld[i][j] = L;
			else if(Y == i || X == j) gameworld[i][j] = L;
			else gameworld[i][j] = E;
			printw("%c",gameworld[i][j]);
		}
		printw("\n");
	}
}

void upt_game()
{
	for(int i = 0;i <= Y;i++)
	{
		for(int j = 0;j <= X;j++)
		{
			if(py == i && px == j) gameworld[i][j] = P;
			else if(gameworld[i][j] != L && gameworld[i][j] != N) gameworld[i][j] = E;
			printw("%c",gameworld[i][j]);
		}
		printw("\n");
	}
}


void gravity()
{
	if(gameworld[py + 1][px] == E)
	{
		py++;
	}
}

int possible_jump()
{
	int z = jump_h;
	while(z--)
	{
		if(gameworld[py - z][px] == L) return 0;
	}
	return 1;
}

void jumping()
{
	if(jump > 0)
	{
		jump--;
		py--;
	}
	else is_jumping = 0;
}

void player_move()
{
	char c = getch();
	switch(c)
	{
		case 'd':
		case 'D':
			if(gameworld[py][px + 1] == E)
			{
				px++;
			}
			break;
		case 'q':
		case 'Q':
			if(gameworld[py][px - 1] == E)
			{
				px--;
			}
			break;
		case ' ':
			if(gameworld[py + 1][px] == L && possible_jump())
			{
				is_jumping = 1;
				jump = jump_h;
			}
			break;
		default:
			break;
	}
}

int random_in_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

void plat_form_gen()
{
	int s = score;
	while(s--)
	{
		int y = random_in_range(Y - 7,Y - 2);
		int x = random_in_range(2,X - 3);
		for(int i = 0;i < 3;i++)
		{
			gameworld[y][x + i] = L;
			gameworld[Y][x + i] = N;
		}
	}
}


int main()
{

	initscr();          // Initialize the ncurses mode
	timeout(0);         // Non-blocking input
	noecho();           // Disable key echoing
    	curs_set(0);        // Hide the cursor

	print_game();
	int gen = 1;

	while(1)
	{
		clear();
		if(px + 1 == X)
		{
			gen = 1;
			px = 1;
			py = Y - 1;
			score++;
			print_game();
		}
		else upt_game();
		if(is_jumping) jumping();
		else gravity();
		player_move();
		if(gen)
		{
			plat_form_gen();
			gen = 0;
		}
		if(gameworld[py + 1][px] == N) break;
		refresh();
		usleep(100000);
	}
	clear();
	endwin();
	printf("Game Over\n\tYou reached level : %d\n",score);
	return 1;
}
