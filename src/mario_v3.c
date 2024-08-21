#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h> 
#include <math.h>

#define L 10
#define W_W 800
#define W_H 600
#define P_W 20
#define P_H 20
#define S_W 60
#define S_H 20
#define MAX_LVL 2

// define the number of states and actions
#define NUM_S 250
#define NUM_A 4 // jump, left, right and do nothing
#define NUM_BINS 20

// learning parameters
#define ALPHA 0.1 // learning rate
#define GAMMA 0.9 // discount factor
float epsilon = 0.1f;     // Initial exploration rate
float epsilon_min = 0.01f; // Minimum value of epsilon
float decay_rate = 0.995f; // Decay factor per episode or step

int px = 1 + L; 
int py = W_H - P_H - L;

int xx = 1 + L; 
int xy = W_H - P_H - L;


int speed = 5;

int jump_h = -25;

int jump = 0;

int xvy = 0;
int pvy = 0;

int gravity = 1;

int ponground = 1;
int xonground = 1;

int quit = 0;

int score = 0;

int xl = 0,xr = 0;
int pl = 0,pr = 0;

int gen = 1;

int platformX[MAX_LVL], platformY[MAX_LVL];

int state;

int action;

// Q-Table
float Q[NUM_S][NUM_A];

void update_epsilon()
{
    	if (epsilon > epsilon_min)
	{
        	epsilon *= decay_rate;
    	}
	else
	{
        	epsilon = epsilon_min;
    	}
}

// initlize the Q-table with zeros
void init_Q_table()
{
	for(int i = 0;i < NUM_S;i++)
	{
		for(int j = 0;j < NUM_A;j++)
		{
			scanf("%f",&Q[i][j]);
//			Q[i][j] = 0.0f;
		}
	}
}

void save_Q_table()
{
	for(int i = 0;i < NUM_S;i++)
	{
		for(int j = 0;j < NUM_A;j++)
		{
			printf("%.2f",Q[i][j]);
		}
		printf("\n");
	}
}

int get_nearest_platform_index()
{
	int min_index = 0;
	int min_d = W_W;
	for(int i = 0;i < score;i++)
	{
	      	int c = platformX[i] - xx;
       		if(c < min_d && c > 0)
		{
			min_d = c;
			min_index = i;
		}	
	}
	return min_index;
}

int discretize_position(int pos,int max_pos,int num_bins)
{
	int bin_size = max_pos / num_bins;
	return pos / bin_size;
}

int discretize_velocity()
{
	if(xvy < 0) return 0; // jumping
	else if(xvy == 0) return 1; // static
	else return 2; // falling
}

void get_state_index()
{
	int i = get_nearest_platform_index();
	int xx_bin = discretize_position(xx, W_W, NUM_BINS);
	int xy_bin = discretize_position(xy, W_H, NUM_BINS);
	int xvy_bin = discretize_velocity();

	int platform_dist_x = discretize_position(platformX[i] - xx
						, W_W, NUM_BINS);
	int platform_dist_y = discretize_position(platformY[i] - xy
						, W_H, NUM_BINS);
	int player_dist = discretize_position(xx - px, W_W, 
						NUM_BINS);

	if(score == 0) platform_dist_x = platform_dist_y = 0;

	state = ((xx_bin * 1000000) + (xy_bin * 100000) + 
		(xvy_bin * 10000) +
		(platform_dist_x * 1000) + (platform_dist_y * 100)
	       	+ player_dist ) 
		% NUM_S;
}


// choose action based on the ε-greedy policy
int choose_action()
{
	if((float)rand() / (float) RAND_MAX < epsilon)
	{
		// explore : choose a random action
		return rand() % 4;
	}
	else
	{
		int best_action = 0;

		float max_value = Q[state][0];

		for(int i = 1;i < NUM_A;i++)
		{
			if(Q[state][i] > max_value)
			{
				max_value = Q[state][i];
				best_action = i;
			}
		}
		return best_action;
	}
}

// update the Q-Table based on the Q-learning formula
void update_Q_table(int action, float reward, int new_state)
{
    	float max_future_q = Q[new_state][0];
    	
	for (int i = 1; i < NUM_A; i++)
	{
        	if (Q[new_state][i] > max_future_q)
		{
            		max_future_q = Q[new_state][i];
        	}
    	}
    	
	Q[state][action] = Q[state][action] + ALPHA * 
		(reward + GAMMA * max_future_q - Q[state][action]);
}


void win_game(int lvl)
{
	score = (lvl > MAX_LVL) ? MAX_LVL : lvl;
	gen = 1;
	px = 1 + L; 
	py = W_H - P_H - L;
	xx = px;
	xy = py;
}

void reset_game(int *x,int *y)
{
	(*x) =	1 + L; 
	(*y) = W_H - P_H - L;
}

void collision(int *x,int *y,int xenon)
{
	if(xenon) xonground = 0;
	else ponground = 0;
    	for(int i = 0; i < score; i++)
    	{   
        	if ((*y) + P_H >= platformY[i] && 
		(*y) + P_H <= platformY[i] + S_H 
		&& // Player's bottom edge is on platform
            	(*x) + P_W > platformX[i] 
		&& (*x) < platformX[i] + S_W) 
			// Player's X position is within platform
        	{
			if(xenon)
			{
				xonground = 0;
				xvy = 0;
			}
			else
			{
            			ponground = 1;
            			pvy = 0; // Stop vertical movement
			}
            		(*y) = platformY[i] - P_H; 
        	}
    	}
}




void game_logic(int *x,int *y,int l,int r,int xenon)
{
	if(l && (*x) > L) (*x) -= speed;
	if(r) (*x) += speed;

	if(xenon)
	{
		xvy += gravity;
		(*y) += xvy;	
		if((*y) > W_H - P_H - L)
		{
			(*y) = W_H - P_H - L;
			xvy = 0;
			xonground = 1;
		}
		else collision(x,y,1);
	}
	else
	{
		pvy += gravity;
		(*y) += pvy;
	
		if((*y) > W_H - P_H - L)
		{
			(*y) = W_H - P_H - L;
			pvy = 0;
			ponground = 1;
		}
		else collision(x,y,0);
	}
}




int lose_game(int *x,int *y)
{
	for(int i = 0;i < score;i++)
	{
        	if ((*y) > platformY[i] + S_H
		&& (*x) + P_W > platformX[i] 
		&& (*x) < platformX[i] + S_W)  
        	{
			return 1;
        	}
	}
	return 0;
}

void game_cnt(SDL_Event *e) 
{
	while(SDL_PollEvent(e) != 0)
	{
		if(e->type == SDL_QUIT || (e->type == SDL_KEYDOWN && 
		e->key.keysym.sym == SDLK_p)) quit = 1;
			// SDL_KEYDOWN handles presses
			// SDL_KEYUP handles releases

		else if (e->type == SDL_KEYDOWN)
        	{
			switch(e->key.keysym.sym)
			{
				case SDLK_q:
					pl = 1;
					break;
				case SDLK_d:
					pr = 1;
					break;
				case SDLK_SPACE:
					if(ponground)
					{
						pvy = jump_h;
						ponground = 0;
					}
					break;

			}
		}
       		else if(e->type == SDL_KEYUP)
		{
			switch(e->key.keysym.sym)
			{
				case SDLK_q:
					pl = 0;
					break;
				case SDLK_d:
					pr = 0;
					break;
			}
		} 
	}

}


int random_in_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}


void generate_fixed_platforms()
{
	switch(score)
	{
		case 1:
			platformX[0] = W_W / 4;
        		platformY[0] = W_H / 2;
			break;
		case 2:
			platformX[0] = W_W / 3;
        		platformY[0] = W_H / 2;
        		platformX[1] = 2 * W_W / 3;
        		platformY[1] = W_H / 3;
			break;
		default:
			break;
	}
}

void generate_platforms()
{
	for(int i = 0; i < score; i++)
	{
		platformX[i] = random_in_range(L + P_W, 
					W_W - S_W - L);
		platformY[i] = random_in_range(W_H/2 
				,W_H - P_H - L - S_H);

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


int calculate_reward()
{
	int player_dist = abs(px - xx);
	if(lose_game(&xx,&xy)) return -100;
	if(xx > W_W - L - P_W) return 100;
	if(xx > px) return 20 + player_dist / 10;
	if(xx <= px) return -10 - player_dist;
	if(xr) return 10;
	if(!xonground) return 1;
	if(!xonground && !xr && !xl) return -5;
	if(xonground && !xl && !xr) return -1;
	return 0;
}



void xenon_move() // xenon is the name i gave to the AI
{
	// update state representation
	get_state_index();

	// decide action based on current state
	action = choose_action();


	// apply the chosen action
	switch(action)
	{
		case 0: // move left
			xl = 1;
			xr = 0;
			break;
		case 1: // move right
			xl = 0;
			xr = 1;
			break;
		case 2: // jump
			if(xonground)
			{
				xvy = jump_h;
				xonground = 0;
			}
			break;
		case 3: // do nothing
			xl = 0;
			xr = 0;
			break;
	}
}





int main(int argc,char* argv[])
{

	freopen("Qtable_input.txt","r",stdin);

	freopen("Qtable_input.txt","w",stdout);


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

	init_Q_table();
	
	win_game(score);

	while(!quit)
	{
		// game logic
		game_logic(&xx,&xy,xl,xr,1);
		game_logic(&px,&py,pl,pr,0);
		
		
		// clear screen
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00,
					0xFF); // Black
		SDL_RenderClear(renderer);
	
		if(px > W_W - L - P_W && xx > W_W - L - P_W)
		{
			win_game(score + 1);
		}

		SDL_Rect xenon = {xx,xy,P_W,P_H};
		SDL_SetRenderDrawColor(renderer,0xFF, 0x00, 0x00,
						0xFF); // Red
		SDL_RenderFillRect(renderer, &xenon);


		SDL_Rect player = {px,py,P_W,P_H};
		
		SDL_SetRenderDrawColor(renderer,0x00, 0x00, 0xFF,
						0xFF); // blue
		SDL_RenderFillRect(renderer, &player);

		
		if(gen)
		{
			generate_platforms();
			gen = 0;
		}

		if(lose_game(&px,&py))
		{
			reset_game(&px,&py);
		}
		else if(lose_game(&xx,&xy))
		{
			reset_game(&xx,&xy);
		}

		// draw platform
		draw_platforms(renderer);

		// Q-learning model
		xenon_move();
		
		int reward = calculate_reward();

		get_state_index();

		int new_state = state;

		update_Q_table(action,reward,new_state);

		// Dynamic ε update
    		update_epsilon();


		// player move
		game_cnt(&e);

		// update the screen
		SDL_RenderPresent(renderer);

		// setting frame rate
		SDL_Delay(16);
	}

	// clean everything
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	save_Q_table();

//	printf("Game Over\n\tYou reached level : %d\n",score);
	return 0;
}


