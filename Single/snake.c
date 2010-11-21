/*
 *  snake.c
 *  SnakeClassical
 *  These are the functions that deal strictly with the functioning of the snakes in my program.
 *  
 *
 *  Created by William Covert III on 12/11/09.
 *  Copyright 2009 Covert Creations. All rights reserved.
 *
 */

#include <math.h>
#include "snake.h"


void edges(SNAKE *snake, int bound_x, int bound_y);
void movesnake(SNAKE *snake, int x, int y);
void eatfood(SNAKE *snake2, FOOD *food, int foo_num);
void checksnake(SNAKE *snake);
void if_loose(SNAKE *snake);
int Food_spawn_check(WORLD world, int x, int y);


//Public Functions Below

//Functions that deal with the food.
void Food_init(WORLD world, FOOD *food)
{
	int i;
	FOOD tfood = *food;
	for(i=0; i<world.food_num; i++)
	{
		tfood.foodeaten=0;
		tfood.loc.x=rand()%(world.boundx-3)+1;
		tfood.loc.y=rand()%(world.boundy-3)+1;
		while (Food_spawn_check(world, tfood.loc.x, tfood.loc.y)) {
			tfood.loc.x=rand()%(world.boundx-3)+1;
			tfood.loc.y=rand()%(world.boundy-3)+1;
		}
		*food=tfood;
		food++;
		tfood = *food;
	}
}

void Food_reset(WORLD world, FOOD *food)
{
	FOOD tfood = *food;
	int i;
	for(i=0; i<world.food_num; i++)
	{
		if(tfood.foodeaten)
		{
			tfood.foodeaten=0;
			tfood.loc.x=rand()%(world.boundx-3)+1;
			tfood.loc.y=rand()%(world.boundy-3)+1;
			while (Food_spawn_check(world, tfood.loc.x, tfood.loc.y)) {
				tfood.loc.x=rand()%(world.boundx-3)+1;
				tfood.loc.y=rand()%(world.boundy-3)+1;
			}
			*food=tfood;
		}
		food++;
		tfood = *food;
	}
}

int Food_spawn_check(WORLD world, int x, int y)
{
	int i,k;
	int in_snake = 0;
	for (k=0; k<world.snake_num; k++) {
		for (i=0; i<world.snake[k].size; i++) {
			if(world.snake[k].body[i].x==x && world.snake[k].body[i].y==y )
				{in_snake=1; k=world.snake_num; i=world.snake[k].size;}
		}
	}
	return in_snake;
}

//Higher Level functions that deals with the snake
void Snake_init(SNAKE *snake, int vel, int startx, int starty, int ON, int AI)
{
	SNAKE tsnake = *snake;
	
	start(&tsnake, vel, startx, starty);
	

	
	tsnake.total_food=0;
	tsnake.high_score=0;
	tsnake.deaths=0;
	
	tsnake.ON=ON;
	tsnake.AI=AI;
	tsnake.movin.x=0;
	tsnake.movin.y=0;
	
	*snake=tsnake;
}

void Snake_GO(SNAKE *snake, int vel, int bound_x, int bound_y, int spawnx, int spawny, FOOD *foo, int foo_num)
{
	SNAKE tsnake = *snake;
	if(tsnake.ON)
	{
		if(tsnake.dead>=1)
		{
			tsnake.dead++;
			//Waits for 50 refreshes before dead snake respawns
			if(tsnake.dead>50)
			{
				//respawns snake
				start(&tsnake, 0, spawnx, spawny);
				tsnake.dead=0;
			}
		}
		else
		{
			movesnake(&tsnake, tsnake.movin.x, tsnake.movin.y);
			eatfood(&tsnake, foo, foo_num);
			edges(&tsnake, bound_x, bound_y);
			checksnake(&tsnake);
		}
	}
	*snake = tsnake;
}
	

void start(SNAKE *snake, int vel,int start_x, int start_y) 
{
	SNAKE tempsnake=*snake;
	int i;
	tempsnake.dead = 0;
	tempsnake.vel = vel;
	tempsnake.size = 4;
	

	tempsnake.body[0].x=start_x;
	tempsnake.body[0].y=start_y;
	for(i=1;i<tempsnake.size;i++){
		tempsnake.body[i].x=start_x-(cos((vel)*M_PI_2));
		tempsnake.body[i].y=start_y-(sin((vel)*M_PI_2));
	}
	
	
	*snake=tempsnake;
}

void hit_each_other(SNAKE *snake, SNAKE *tenemy)
{
	//Checks to see if the snake is in another snake and sets loose to one if it is.
	int i;
	SNAKE tsnake = *snake;
	SNAKE enemy = *tenemy;
	if (tsnake.dead==0 && enemy.dead==0) 
	{
		if (tsnake.body[0].x == enemy.body[0].x && tsnake.body[0].y == enemy.body[0].y) {
			tsnake.dead =1;
			tsnake.deaths++;
			enemy.dead =1;
			enemy.deaths++;
		}
	}
	if (tsnake.dead==0 && enemy.dead==0) 
	{
		for (i=1; i<enemy.size; i++) {
			if (tsnake.body[0].x == enemy.body[i].x && tsnake.body[0].y == enemy.body[i].y) {
				tsnake.dead =1;
				tsnake.deaths++;
			}
		}
	}
	*snake=tsnake;
	*tenemy=enemy;
}

	
//Lower level functions that deal with the snake
void movesnake(SNAKE *snake, int x, int y) 
{
	int i;
	
	SNAKE prev_snake1=*snake;      //Stores the previous state
	SNAKE cur_snake = prev_snake1;  //Store the current state from the previous
	
	//Change Vel if Keys are pressed
	if (x == -1 && cur_snake.vel != 0) {
		cur_snake.vel = 2;
	}
	else if (x == 1 && cur_snake.vel != 2) {
		cur_snake.vel = 0;
	}
	if (y == -1 && x==0 && cur_snake.vel != 1) {
		cur_snake.vel = 3;
	}
	else if (y == 1 && x==0 && cur_snake.vel != 3) {
		cur_snake.vel = 1;
	}
	
	//Update Head
	switch(cur_snake.vel) {
		case 0:
			cur_snake.body[0].x += 1;
			break;
		case 1:
			cur_snake.body[0].y += 1;
			break;
		case 2:
			cur_snake.body[0].x -= 1;
			break;
		case 3:
			cur_snake.body[0].y -= 1;
			break;
	}
	
	
	//Update the Rest to the position of the one ahead of it
	//This is where is store current state from previous state
	for (i=1; i<cur_snake.size; i++) {
		cur_snake.body[i]=prev_snake1.body[i-1];
	}
	
	//Switch from temp to snake
	*snake=cur_snake;  //Since temp snake has the current values
}

void eatfood(SNAKE *snake2, FOOD *food, int foo_num)
{
	SNAKE snake1 = *snake2;
	FOOD foo = *food;
	int i;
	for(i=0; i<foo_num; i++)
	{
		if (snake1.body[0].x == foo.loc.x && snake1.body[0].y == foo.loc.y) {
			int i;
			foo.foodeaten=1;
			snake1.total_food++;
			
			//This updates the size of the snake and places all the new addons in the last
			//square... gets straigthened out in movesnake().
			
			int p_size = snake1.size;
			
			snake1.size+=5;
			
			if(snake1.high_score<snake1.size)
				{snake1.high_score=snake1.size;}
			
			for(i=p_size; i<snake1.size; i++){
				snake1.body[i] = snake1.body[p_size-1];
			}
			
		}
		*food=foo;
		food++;
		foo = *food;
	}
	
	*snake2=snake1;
}

void checksnake(SNAKE *snake)
{
	//Check snake sees it the snake head is in any other body part
	//If it is it sets snake.dead to one to let the program know that
	//the snake is in itself
	
	int i;
	SNAKE snake1= *snake;
	for (i=1; i<snake1.size; i++) {
		if (snake1.body[0].x == snake1.body[i].x && snake1.body[0].y == snake1.body[i].y) {
			snake1.dead =1;
			snake1.deaths++;
		}
	}
	*snake=snake1;
}

void edges(SNAKE *snake, int bound_x, int bound_y)
{
	//Checks to see if the snake is out of bounds
	//Sets snake.dead to one if it is.
	SNAKE snake1= *snake;
	if (snake1.body[0].x>(bound_x-1) || snake1.body[0].x<0 || snake1.body[0].y>(bound_y-1) || snake1.body[0].y<0){
		snake1.dead = 1;
		snake1.deaths++;
	}
	*snake=snake1;
}
