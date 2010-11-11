/*
 *  world.h
 *  Snake Simple
 *
 *  Created by William Covert III on 5/24/10.
 *  Copyright 2010 Covert Creations. All rights reserved.
 *
 */


typedef struct {
	int x;
	int y;
	int id;
	char name[25];
} vi2;

typedef struct {
	int x,y;
} vector2;

typedef struct {
	int send;
	int foodeaten;
	vector2 loc;
} FOOD;

typedef struct {
	int send_vel;
	int send_dead;
	int send_start;
	int send_size;
	
	int AI;
	int ON;
	
	int dead;
	
	int high_score;
	int total_food;
	int deaths;
	
	vector2 movin;
	int vel;  
	int size;
	int sizep;
	vector2 body[500];
} SNAKE;

typedef struct {
	int boundx;
	int boundy;
	int paused;
	
	float gametimer;
	int maxtime;
	
	int snake_num;
	SNAKE snake[50];
	
	int food_num;
	FOOD food[50];
} WORLD;

#include "snake.h"

//Initilizes the world -- needs modified
void World_init(WORLD *world, int sizex, int sizey, int time, int foodNum);

//Moves the world ahead one step
void World_step(WORLD *world);

//Client File
void movesnake_client(SNAKE *snake) ;