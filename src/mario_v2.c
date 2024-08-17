#include <stdio.h>
#include <SDL2/SDL.h>

#define L 10
#define W_W 800
#define W_H 600
#define P_W 20
#define P_H 20
#define S_W 60
#define S_H 20


int px = 1 + L; 
int py = W_H - P_H - L;
int speed = 5;

int jump_h = -20;

int jump = 0;

int vy = 0;

int gravity = 1;

int onground = 1;

int quit = 0;

int score = 1;

int l = 0,r = 0;


int platformX[S_W], platformY[S_H];


void collision()
{
	onground = 0; // Assume the player is not on the ground
    	for(int i = 0; i < score; i++)
    	{   
        	if (py + P_H >= platformY[i] && 
		py + P_H <= platformY[i] + S_H 
		&& // Player's bottom edge is on platform
            	px + P_W > platformX[i] && px < platformX[i] + S_W) 
			// Player's X position is within platform
        	{
            		onground = 1;
            		vy = 0; // Stop vertical movement
            		py = platformY[i] - P_H; 
        	}
    	}
}

void lose_game()
{
	for(int i = 0;i < score;i++)
	{
        	if (py > platformY[i] + S_H
		&& px + P_W > platformX[i] 
		&& px < platformX[i] + S_W) 
        	{
			quit = 1;
        	}
	}
}

void player_move(SDL_Event *e) 
{
	while(SDL_PollEvent(e) != 0)
	{
		if(e->type == SDL_QUIT || (e->type == SDL_KEYDOWN && 
		e->key.keysym.sym == SDLK_p)) quit = 1;
			// SDL_KEYDOWN handles presses
			// SDL_KEYUP handles releases
		else if(e->type == SDL_KEYDOWN)
		{
			switch(e->key.keysym.sym)
			{
				case SDLK_q:
					l = 1;
					break;
				case SDLK_d:
					r = 1;
					break;
				case SDLK_SPACE:
					if(onground)
					{
						vy = jump_h;
						onground = 0;
					}
					break;

			}
		}
		else if(e->type == SDL_KEYUP)
		{
			switch(e->key.keysym.sym)
			{
				case SDLK_q:
					l = 0;
					break;
				case SDLK_d:
					r = 0;
					break;
			}
		}
	}
	if(l && px > L) px -= speed;
	if(r) px += speed;


	vy += gravity;
	py += vy;
	
	if(py > W_H - P_H - L)
	{
		py = W_H - P_H - L;
		vy = 0;
		onground = 1;
	}
	else collision();
}


int random_in_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}


void generate_platforms()
{
	for(int i = 0; i < score; i++)
	{
		platformX[i] = random_in_range(L + P_W, 
					W_W - S_W - L);
		platformY[i] = random_in_range(W_H - P_H - L - S_H
				+ jump_h ,W_H - P_H - L - S_H);

		// range is not working ?????
	}
}


void draw_platforms(SDL_Renderer *renderer)
{
	for(int i = 0; i < score; i++)
	{
		SDL_Rect platform = {platformX[i], platformY[i], 
					S_W, S_H};
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 
					0x00, 0xFF); // Green
		SDL_RenderFillRect(renderer, &platform);
	}
}


int main(int argc,char* argv[])
{
	//initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL coudn't start");
		return 1;
	}

	// create window
	SDL_Window* window = SDL_CreateWindow("My SDL2 Game",
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					W_W,W_H,SDL_WINDOW_SHOWN);
	if(window == NULL)
	{
		printf("window coudn't be created");
		SDL_Quit();
		return 1;
	}

	// create renderer ( the renderer draw everything )
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,
					SDL_RENDERER_ACCELERATED);
	if(renderer == NULL)
	{
		printf("Renderer couldn't be created");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}


	// Event handler ( handle keys and window closing )
	SDL_Event e;
	
	int gen = 1;

	while(!quit)
	{
		player_move(&e);

		// clear screen
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00,
					0xFF); // Black
		SDL_RenderClear(renderer);
	
		if(px > W_W - L - P_W)
		{
			gen = 1;
			px = 1 + L; 
			py = W_H - P_H - L;
			score++;
		}

		SDL_Rect player = {px,py,P_W,P_H};
		SDL_SetRenderDrawColor(renderer,0xFF, 0x00, 0x00,
						0xFF); // Red
		SDL_RenderFillRect(renderer, &player);
		
		if(gen)
		{
			generate_platforms();
			gen = 0;
		}

		lose_game();

		// draw platform
		draw_platforms(renderer);

		// update the screen
		SDL_RenderPresent(renderer);

		// setting frame rate
		SDL_Delay(16);
	}

	// clean everything
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("Game Over\n\tYou reached level : %d\n",score);
	return 0;
}

