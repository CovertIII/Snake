/*
 *  snake.h
 *  SnakeClassical
 *
 *  Created by William Covert III on 12/11/09.
 *  Copyright 2009 New Tribes Bible Institute. All rights reserved.
 *
 */

// SNAKE.vel indicates the direction the snake is going.
// 0 is →, 1 is ↑, 2 is ←, 3 is ↓; If you multiply these by
//   pi/2 (M_PI_2 if you have math.h included) 
//   then you have it's direction in radians.

void Food_init(WORLD world, FOOD *food);
void Food_reset(WORLD world, FOOD *food);

void Snake_init(SNAKE *snake, int vel, int bound_x, int bound_y, int ON, int AI);
void Snake_GO(SNAKE *snake, int vel, int bound_x, int bound_y, int spawnx, int spawny, FOOD *foo, int foo_num);

void start(SNAKE *snake, int vel,int start_x, int start_y);
void hit_each_other(SNAKE *snake, SNAKE *enemy);