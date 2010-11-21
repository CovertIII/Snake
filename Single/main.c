#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include "snake.h"

#define scale 10  //Size in pixels of the snake body squares
#define game_speed 0.05 //In seconds how fast the game refreshes.
#define game_time 60
#define sc_width 800
#define sc_height 600


int lastFrameTime = 0;
float timecheck = 0;

WORLD myworld;


void square(int x, int y){
	glPushMatrix();
	glTranslatef(x*scale,y*scale,0);
	glBegin(GL_POLYGON);
	glVertex2f(0,0);
	glVertex2f(scale-1,0);
	glVertex2f(scale-1,scale-1);
	glVertex2f(0,scale-1);
    glEnd();
	glPopMatrix();
}

void renderBitmapString(float x, float y, void *font, char *string) 
{  
	glPushMatrix();
	glLoadIdentity();
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
	glPopMatrix();
}

void init(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	World_init(&myworld, sc_width, sc_height, 2, 0);
}
void calc(void){
	rand();
		
	myworld.boundx = glutGet(GLUT_WINDOW_WIDTH)/scale;
	myworld.boundy = (glutGet(GLUT_WINDOW_HEIGHT)-90)/scale;
	
	int now = glutGet(GLUT_ELAPSED_TIME);
  int elapsedMilliseconds = now - lastFrameTime;
  float elapsedTime = elapsedMilliseconds / 1000.0f;
  lastFrameTime = now;
	float h = elapsedTime;
	
	timecheck += h;
	if(!myworld.paused && myworld.gametimer <= game_time){myworld.gametimer+=h;}
	if (timecheck > game_speed && myworld.paused ==0 && myworld.gametimer <= game_time)
	{
		timecheck = 0;
		World_step(&myworld);
	}
}

void render(void)
{
	int i,k;
	//-----This is the stuff involved with drawing the screen----//	
	glClearColor (0.6, 0.6,0.6,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//---------------SNAKE1---------------
	//Displays snakes and relevent information.
	for(k=0; k<myworld.snake_num; k++)
	{
		float r=k/6.0f;
		float g=0.2f;
		float b=1-k/6.0f;
		if(myworld.snake[k].ON)
		{
			
			if(myworld.snake[k].dead==00)
			{
				glColor3f (r, g, b);
				for (i=0; i<myworld.snake[k].size; i++) {square(myworld.snake[k].body[i].x, myworld.snake[k].body[i].y);}
			}
			else if (myworld.snake[k].dead < 20) 
			{
				glColor4f (r*0.5, g*0.5, b*0.5, 0.75);
				for (i=0; i<myworld.snake[k].size; i++) {square(myworld.snake[k].body[i].x, myworld.snake[k].body[i].y);}
			}
			char buf[30];
			int temp1;
			glColor3f (r, g, b);
			temp1 = myworld.snake[k].total_food;
			sprintf(buf, "Total Food: %i", temp1);	
			renderBitmapString(0 + k*130, glutGet(GLUT_WINDOW_HEIGHT)-30, GLUT_BITMAP_HELVETICA_18, buf);
			

			temp1 = myworld.snake[k].deaths;
			sprintf(buf, "Total Deaths: %i", temp1);	
			renderBitmapString(0 + k*130, glutGet(GLUT_WINDOW_HEIGHT)-50, GLUT_BITMAP_HELVETICA_18, buf);
			
			
			temp1 = myworld.snake[k].total_food-myworld.snake[k].deaths;
			sprintf(buf, "Score: %i", temp1);	
			renderBitmapString(0 + k*130, glutGet(GLUT_WINDOW_HEIGHT)-70, GLUT_BITMAP_HELVETICA_18, buf);
		}
		
	}
	
		char buf[30];
		float average;
		for(i=0;i<myworld.snake_num; i++)
		{
			average += myworld.snake[i].total_food-myworld.snake[i].deaths;
		}
		average = average/myworld.snake_num;
		sprintf(buf, "Average Score: %.1f", average);	
		renderBitmapString(0 + myworld.snake_num*130, glutGet(GLUT_WINDOW_HEIGHT)-70, GLUT_BITMAP_HELVETICA_12, buf);
	
	//Displays Food
	glColor3f (0.196078, 0.803922, 0.196078);
	for(i=0;i<myworld.food_num;i++)
	{
		square(myworld.food[i].loc.x, myworld.food[i].loc.y);
	}
	
	
	//Displays Boundaries
	glColor3f (0, 0, 0);
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f (0,scale*myworld.boundy, 0.0);
	glVertex3f (scale*myworld.boundx,scale*myworld.boundy, 0.0);
	glVertex3f (scale*myworld.boundx,scale*myworld.boundy, 0.0);
	glVertex3f (scale*myworld.boundx,0, 0.0);
    glEnd();
	glPopMatrix();
	
	//Display's Timer
	int temp1 = game_time-myworld.gametimer;
	sprintf(buf, "Timer: %d:%.2d", temp1/60, temp1%60);	
	renderBitmapString(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)-90, GLUT_BITMAP_HELVETICA_18, buf);

	

	//Displays pause if game is paused
	if(myworld.paused)
	{
		renderBitmapString(glutGet(GLUT_WINDOW_WIDTH)/2-100, 	glutGet(GLUT_WINDOW_HEIGHT)/2, GLUT_BITMAP_TIMES_ROMAN_24, "GAME PAUSED");	
	}
	
	glutSwapBuffers();
	glFlush();
}


void display(void) 
{
	calc();
	render();
}

void processNormalKeys(unsigned char key, int xx, int yy) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'P':
		case 'p':
			if(!myworld.paused) {myworld.paused = 1;}
			else {myworld.paused = 0;}
			break;
		case 'R':
		case 'r':
			if(myworld.paused || myworld.gametimer>=game_time)
			{
				myworld.gametimer = 0;
				World_init(&myworld, sc_width, sc_height, 5, 0);
			}
			break;
			
		default:
			break;
	}
}


void pressKey(int key, int xx, int yy) {
	switch(key) {
		case GLUT_KEY_LEFT : 
			myworld.snake[0].movin.x=-1;
			break;
		case GLUT_KEY_RIGHT : 
			myworld.snake[0].movin.x= 1;
			break;
		case GLUT_KEY_UP : 
			myworld.snake[0].movin.y= 1;
			break;
		case GLUT_KEY_DOWN : 
			myworld.snake[0].movin.y=-1;
			break;
	}
}

void releaseKey(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_LEFT :
			if(myworld.snake[0].movin.x<0){myworld.snake[0].movin.x=0;}
			break;
		case GLUT_KEY_RIGHT : 
			if(myworld.snake[0].movin.x>0){myworld.snake[0].movin.x=0;}
			break;
		case GLUT_KEY_UP :
			if(myworld.snake[0].movin.y>0){myworld.snake[0].movin.y=0;}
			break;
		case GLUT_KEY_DOWN :
			if(myworld.snake[0].movin.y<0){myworld.snake[0].movin.y=0;}
			break;
		default:
			break;
	}
}


void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(sc_width, sc_height);
  glutCreateWindow("Snake");
	
	init();
	
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey); 

	glutDisplayFunc(display);
  glutReshapeFunc(reshape);
	glutIdleFunc(idle);
    
	
    glutMainLoop();
    return EXIT_SUCCESS;
}
