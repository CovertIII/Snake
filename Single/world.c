/*
 *  world.c
 *  Snake Simple
 *
 *  Created by William Covert III on 5/24/10.
 *  Copyright 2010 Covert Creations. All rights reserved.
 *
 */

#include "world.h"


void World_init(WORLD *world, int sizex, int sizey, int snakes, int play)
{
	int i;
	WORLD tworld = *world;
	
	tworld.boundx = sizex/12;
	tworld.boundy = (sizey-90)/12;
	tworld.paused = 1;
	tworld.gametimer = 0;
	tworld.snake_num = snakes;
	tworld.food_num = 10;
	
	
	for(i=0; i<tworld.snake_num; i++){
		if(i==0){Snake_init(&tworld.snake[i], 0, 5, tworld.boundy-tworld.boundy/tworld.snake_num*i-tworld.boundy/(tworld.snake_num*2), 1, play);}
		else{Snake_init(&tworld.snake[i], 0, 5, tworld.boundy-tworld.boundy/tworld.snake_num*i-tworld.boundy/(tworld.snake_num*2), 1, 1);}
	}
	Food_init(tworld, tworld.food);
	
	*world=tworld;
}

void World_step(WORLD *world)
{
	int i,k;
	WORLD tworld = *world;
	
	for(i=0; i<tworld.snake_num; i++)
	{
		if(tworld.snake[i].AI){tworld.snake[i].movin = snake_AI(tworld, i);}
		Snake_GO(&tworld.snake[i], 0, tworld.boundx, tworld.boundy, 5, tworld.boundy-tworld.boundy/tworld.snake_num*i-tworld.boundy/(tworld.snake_num*2), &tworld.food, tworld.food_num);
		for (k=0; k<tworld.snake_num; k++) 
		{
			if(k!=i){hit_each_other(&tworld.snake[i], &tworld.snake[k]);}
		}
		
	}
	

	Food_reset(tworld, tworld.food);
	
	*world=tworld;
}