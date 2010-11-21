/*
 *  AI_Bill.c
 *  Snake Simple
 *
 *  Created by William Covert III on 5/24/10.
 *  Copyright 2010 Covert Creations. All rights reserved.
 *
 */

#include "world.h"
#include <math.h>

/*-----------------------------------------------------------
 This next part deals with the AI and it's functions
 Needs a more work.
 -----------------------------------------------------------*/
float dist(vector2 a, vector2 b)
{
	float c;
	c = sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
	return c;
}

int close_food(WORLD world, int snake_id)
{
	int i,k,h;
	for(k=2; k<world.boundx;k+=4)
	{
		for(i=0;i<world.food_num;i++)
		{
			if(dist(world.snake[snake_id].body[0],world.food[i].loc)<k)
			{
				return i;
				i=world.food_num;
				k=world.boundx;
			} 
		}	
	}
}

vector2 rotate(int ang)
{
	vector2 p;
	switch(ang){
		case -1:
			p.x =  0;
			p.y = -1;
			break;
		case 0:
			p.x = 1;
			p.y = 0;
			break;	
		case 1:
			p.x = 0;
			p.y = 1;
			break;
		case 2:
			p.x = -1;
			p.y = 0;
			break;		
		case 3:
			p.x =  0;
			p.y = -1;
			break;		
		case 4:
			p.x = 1;
			p.y = 0;
			break;		
	}
	return p;
}

//void snake_check(int ang, vector2 head, int xx, int yy, int bound_x, int bound_y, SNAKE check)
vector2 snake_check(WORLD world, int snake_id, int px, int py, int check_id)
{
	//snake_check(snake.vel, snake.body[0], x, y,  bound_x,  bound_y, snake);
	int ang = world.snake[snake_id].vel;
	
	vector2 head = world.snake[snake_id].body[0];
	
	int xx = px;
	int yy = py;
	
	int w_ang = 2*atan2(py,px)/M_PI_2;
	
	int bound_x = world.boundx;
	int bound_y = world.boundy;
	
	SNAKE check = world.snake[check_id];
	
	int i, front=0, side_p=0, side_m=0;
	int loopi=0;
	int x,y,xp,yp,xm,ym;
	
	vector2 n,p,m;
	
	n = rotate(ang); 
	p= rotate(ang+1); 
	m= rotate(ang-1); 
	
	x = n.x; y = n.y; xp = p.x; yp = p.y; xm = m.x; ym = m.y;
	
	for(i=1; i<check.size; i++)
	{
		int j;
		for (j=2; j<=0.5*check.size-2; j++) {
			if(loopi==0 && head.x+xp*j==check.body[i].x && head.y+yp*j==check.body[i].y)
			{
				loopi=i;
			}
			if(loopi==0 && head.x+xm*j==check.body[i].x && head.y+ym*j==check.body[i].y)
			{
				loopi=-i;
			}
		}
		if (head.x+x==check.body[i].x && head.y+y==check.body[i].y)
		{
			//printf("WATCH OUT!!\n");
			front=1;
		}
		if(head.x+xp==check.body[i].x && head.y+yp==check.body[i].y)
		{
			//printf("STAY1!!\n");
			side_p=1;
		}
		if(head.x+xm==check.body[i].x && head.y+ym==check.body[i].y)
		{
			//printf("STAY2!!\n");
			side_m=1;
		}
	}
	
	if(side_p && (xm==px && ym == py)){
		xx=xm; 
		yy=ym;
	}
	else if(side_m && (xp==px && yp == py)){
		xx=xp; 
		yy=yp;
	}
	else if( (side_p && (xp==px && yp == py))||(side_m && (xm==px && ym == py)) ){
		xx=x; 
		yy=y;
	}
	else if(side_p && side_m){
		xx=x; 
		yy=y;
	}
	
	if(front && loopi>0){
		xx=xm; 
		yy=ym;
	}
	else if(front && loopi<0){
		xx=xp; 
		yy=yp;
	}
	else if(front){
		if(rand()%2){xx=xm; yy=ym;}
		else{xx=xp; yy=yp;}
	}
	
	
	//Deals with corners of snakes
	if(front && side_p)
	{
		xx=xm; yy=ym;
	}
	else if(front && side_m)
	{
		xx=xp; yy=yp;
	}
	
	//Head on colisions with other snakes
	if(head.x+x*2==check.body[0].x && head.y+y*2==check.body[0].y){
		if(side_p){xx=xm; yy=ym;}
		else if(side_m){xx=xp; yy=yp;}
		else
		{
			if(rand()%2){xx=xm; yy=ym;}
			else{xx=xp; yy=yp;}	
		}
	}
	
	if(head.x+x+xp==check.body[0].x && head.y+y+yp==check.body[0].y){
		xx=xm; yy=ym;
	}
	else if(head.x+x+xm==check.body[0].x && head.y+y+ym==check.body[0].y){
		xx=xp; yy=yp;
	}
	
	//Deals with boundaries and sides
	if (side_m || side_p) {
		switch (ang) {
			case 0:
				if(head.x==bound_x-1 && side_p){xx=xm; yy=ym;}
				if(head.x==bound_x-1 && side_m){xx=xp; yy=yp;}                                    
				break;                                 
			case 1:                                    
				if(head.y==bound_y-1 && side_p){xx=xm; yy=ym;}
				if(head.y==bound_y-1 && side_m){xx=xp; yy=yp;}
				break;
			case 2:
				if(head.x==0 && side_p){xx=xm; yy=ym;}
				if(head.x==0 && side_m){xx=xp; yy=yp;}
				break;
			case 3:
				if(head.y==0 && side_p){xx=xm; yy=ym;}
				if(head.y==0 && side_m){xx=xp; yy=yp;}
				break;
			default:
				break;
		}
	}
	vector2 r;
	r.x=xx;
	r.y=yy;
	return r;
}

vector2 eat_food(WORLD world, int snake_id)
{
	int food_id;
	food_id=close_food(world, snake_id);
	int difx = world.snake[snake_id].body[0].x - world.food[food_id].loc.x;
	int dify = world.snake[snake_id].body[0].y - world.food[food_id].loc.y;
	vector2 r = world.snake[snake_id].movin;
	int x,y;
	
	x = r.x;
	y = r.y;
	
	/* Food Check
	 *  I have a home a piece of paper with scrible on it that makes sense of all of this, lol
	 *  Basically it "sees" where the food is and acts on it: 
	 *
	 *  The first picture is the different possibilites of where the food can be relitive to the 
	 *  snake.  The second shows the different directions I told it to go if the snakes vel is 0.
	 *  To find the rest I simply rotated the picture. 
	 *
	 *                 8   7   6        ↑   ↑   → 
	 *
	 *                 1   ->  5        ↓   ⇒   →
	 * 
	 *                 2   3   4        ↓   ↓   →
	 *                          
	 *
	 *
	 */
	switch (world.snake[snake_id].vel) 
	{
		case 0:
			if(dify == 0 && difx  > 0){x= 0; y=-1;} //1
			else if(dify >  0 && difx  > 0){x= 0; y=-1;} //2
			else if(dify >  0 && difx == 0){x= 0; y=-1;} //3
			else if(dify >  0 && difx  < 0){x= 1; y= 0;} //4
			else if(dify == 0 && difx  < 0){x= 1; y= 0;} //5
			else if(dify <  0 && difx  < 0){x= 1; y= 0;} //6
			else if(dify <  0 && difx == 0){x= 0; y= 1;} //7
			else if(dify <  0 && difx  > 0){x= 0; y= 1;} //8
			break;
		case 1:
			if(dify == 0 && difx  > 0){x=-1; y= 0;} //1
			else if(dify >  0 && difx  > 0){x=-1; y= 0;} //2
			else if(dify >  0 && difx == 0){x= 1; y= 0;} //3
			else if(dify >  0 && difx  < 0){x= 1; y= 0;} //4
			else if(dify == 0 && difx  < 0){x= 1; y= 0;} //5
			else if(dify <  0 && difx  < 0){x= 0; y= 1;} //6
			else if(dify <  0 && difx == 0){x= 0; y= 1;} //7
			else if(dify <  0 && difx  > 0){x= 0; y= 1;} //8
			break;                                
		case 2:                                   
			if(dify == 0 && difx  > 0){x=-1; y= 0;} //1
			else if(dify >  0 && difx  > 0){x=-1; y= 0;} //2
			else if(dify >  0 && difx == 0){x= 0; y=-1;} //3
			else if(dify >  0 && difx  < 0){x= 0; y=-1;} //4
			else if(dify == 0 && difx  < 0){x= 0; y= 1;} //5
			else if(dify <  0 && difx  < 0){x= 0; y= 1;} //6
			else if(dify <  0 && difx == 0){x= 0; y= 1;} //7
			else if(dify <  0 && difx  > 0){x=-1; y= 0;} //8
			break;                                
		case 3:                                   
			if(dify == 0 && difx  > 0){x=-1; y= 0;} //1
			else if(dify >  0 && difx  > 0){x= 0; y=-1;} //2
			else if(dify >  0 && difx == 0){x= 0; y=-1;} //3
			else if(dify >  0 && difx  < 0){x= 0; y=-1;} //4
			else if(dify == 0 && difx  < 0){x= 1; y= 0;} //5
			else if(dify <  0 && difx  < 0){x= 1; y= 0;} //6
			else if(dify <  0 && difx == 0){x=-1; y= 0;} //7
			else if(dify <  0 && difx  > 0){x=-1; y= 0;} //8
			break;
	}
	r.x = x;
	r.y = y;
	return r;
}

//void AI(SNAKE snake, int *x, int *y, int bound_x, int bound_y, vector2 food)
vector2 snake_AI(WORLD world, int snake_id)
{
	vector2 r;
	int i;
	r = eat_food(world, snake_id);
	//Checks to see if its going to his itself or the walls
	for(i=0; i<world.snake_num; i++)
	{
		if(i!=snake_id){r = snake_check(world, snake_id, r.x , r.y, i);}
	}
	r = snake_check(world, snake_id, r.x , r.y, snake_id);
	
	return r;
}