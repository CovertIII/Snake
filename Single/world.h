/*
 *  world.h
 *  Snake Simple
 *
 *  Created by William Covert III on 5/24/10.
 *  Copyright 2010 Covert Creations. All rights reserved.
 *
 */
typedef struct {
	int x,y;
} vector2;

typedef struct {
	int foodeaten;
	vector2 loc;
} FOOD;

typedef struct {
	int AI;
	int ON;
	
	int dead;
	
	int high_score;
	int total_food;
	int deaths;
	
	vector2 movin;
	int vel;  
	int size;
	vector2 body[500];
} SNAKE;

typedef struct {
	int boundx;
	int boundy;
	int paused;
	
	float gametimer;
	
	int snake_num;
	SNAKE snake[50];
	
	int food_num;
	FOOD food[50];
} WORLD;


//Initilizes the world -- needs modified
void World_init(WORLD *world, int sizex, int sizey, int snakes, int play);

//Moves the world ahead one step
void World_step(WORLD *world);

//AI files
vector2 snake_AI(WORLD world, int snake_id); 