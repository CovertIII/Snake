/*
 *  world.c
 *  Snake Simple
 *
 *  Created by William Covert III on 5/24/10.
 *  Copyright 2010 Covert Creations. All rights reserved.
 *
 */

#include "world.h"

void World_init(WORLD *world, int sizex, int sizey, int time, int foodNum)
{
	WORLD tworld = *world;
	
	tworld.boundx = sizex;
	tworld.boundy = sizey;
	tworld.paused = 1;
	tworld.gametimer = 0;
	tworld.maxtime = time;
	tworld.snake_num = 0;
	tworld.food_num = foodNum;
	
	/*
	for(i=0; i<tworld.snake_num; i++){
		if(i==0){Snake_init(&tworld.snake[i], 0, 5, tworld.boundy-tworld.boundy/tworld.snake_num*i-tworld.boundy/(tworld.snake_num*2), 1, play);}
		else{Snake_init(&tworld.snake[i], 0, 5, tworld.boundy-tworld.boundy/tworld.snake_num*i-tworld.boundy/(tworld.snake_num*2), 1, 1);}
	}*/
	
	Food_init(tworld, tworld.food);
	
	*world=tworld;
}

void World_step(WORLD *world)
{
	int i,k;
	WORLD tworld = *world;
	
	for(i=0; i<tworld.snake_num; i++)
	{
		Snake_GO(&tworld.snake[i], 0, tworld.boundx, tworld.boundy, 5, tworld.boundy-tworld.boundy/tworld.snake_num*i-tworld.boundy/(tworld.snake_num*2), &tworld.food, tworld.food_num);
		for (k=0; k<tworld.snake_num; k++) 
		{
			if(k!=i){hit_each_other(&tworld.snake[i], &tworld.snake[k]);}
		}
		
	}
	Food_reset(tworld, tworld.food);
	
	*world=tworld;
}