#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLUT/glut.h>
#include <enet/enet.h>
#include "world.h"

int scale = 7;

int lastFrameTime = 0;
float timecheck = 0;
float game_speed = 0.05;

vi2 movin;
WORLD myWorld;
WORLD dispWorld;
WORLD wbuffer[15];
int wbuf_num=0;

char buf[100];
int buf_num = 0;

char trans[100];
int tran_in = 0;
int connection = 0;

char disp[100];
char disp1[100];
char disp2[100];

ENetHost * client;
ENetAddress address;
ENetEvent event;
ENetPeer *peer;

void interpret_packet(int type, char *ptr);

int init_network(void){
	if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
	atexit (enet_deinitialize);

	client = enet_host_create (NULL /* create a client host */,
							   1 /* only allow 1 outgoing connection */,
							   2 /* allow up 2 channels to be used, 0 and 1 */,
							   57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
							   14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
   
   if (client == NULL)
   {
       fprintf (stderr, 
                "An error occurred while trying to create an ENet client host.\n");
       exit (EXIT_FAILURE);
   }
	char temp[100];
	//scanf("%s", temp);
}

void square(int x, int y){
	glPushMatrix();
	glTranslatef(x*scale,y*scale,0);
	glBegin(GL_POLYGON);
	glVertex3f (0,0, 0.0);
	glVertex3f (scale-1,0, 0.0);
	glVertex3f (scale-1,scale-1, 0.0);
	glVertex3f (0,scale-1, 0.0);
    glEnd();
	glPopMatrix();
}

void type(unsigned char key) 
{
	int i;
	if(key==10 || key==13)
	{
		buf[buf_num] = NULL;
		strcpy(trans,buf);
		buf_num=0;
		tran_in = 1;
	}
	else{
		for (i=1; i<127; i++) {
			if (key==i && buf_num<97) {
				buf[buf_num]=i;
				buf_num++;
			}
		}
	}
	if (key == 127 && buf_num>0) {
		buf_num--;
	}
}

void renderBitmapString(float x, float y, void *font, char *string) 
{  
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}	

void processNormalKeys(unsigned char key, int xx, int yy) {
	if(key=='+')
	{
		scale++;
	}
	else if(key=='-')
	{
		if(scale>0){scale--;}
	}
	if (key == 27)
	{
		if(connection){
			enet_peer_disconnect (peer, 0);
        	
			    /* Allow up to 3 seconds for the disconnect to succeed
			 and drop any packets received packets.
			     */
			while (enet_host_service (client, & event, 3000) > 0)
			{
			    switch (event.type)
			    {
			    case ENET_EVENT_TYPE_RECEIVE:
			        enet_packet_destroy (event.packet);
			        break;
        	
			    case ENET_EVENT_TYPE_DISCONNECT:
			        printf("Disconnection succeeded.");
					exit(0);
			        return;
			    }
			}
        	
			/* We've arrived here, so the disconnect attempt didn't */
			/* succeed yet.  Force the connection down.             */
			enet_peer_reset (peer);
			exit(0);
		}
		else{exit(0);}
	} 
		
	type(key);
}


void numbers(void){
	int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;
	float h = elapsedTime;
	
	
	timecheck += h;
	if (timecheck > 0.02){
		int i;
		timecheck = 0;
		
		dispWorld = wbuffer[0];
		
		if(wbuf_num == 0)
		{
			dispWorld = myWorld;
		}
		
		if(wbuf_num>0){
			for(i=0; i<wbuf_num; i++){
				wbuffer[i]=wbuffer[i+1];
			}
			wbuf_num--;
		}
	}
	
	
	if (now%1000 < 500) {
		buf[buf_num] = 32;
	}
	else {
		buf[buf_num] = 95;
	}	
		
	
	if(tran_in && connection==0)
	{	
		 enet_address_set_host (& address, trans);
		    address.port = 5001;


		    /* Initiate the connection*/
		    peer = enet_host_connect (client, & address,2, 0);    

		    if (peer == NULL)
		    {
		       fprintf (stderr, 
		                "No available peers for initiating an ENet connection.\n");
		       exit (EXIT_FAILURE);
		    }

		    /* Wait up to 5 seconds for the connection attempt to succeed. */
		    if (enet_host_service (client, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		    {
				sprintf(disp, "Connection to %s succeeded.\n", trans);
				printf("Connection to %s succeeded.\n", trans);
				connection=1;
				tran_in=0;
		    }
		    else
		    {
		        enet_peer_reset (peer);
		        sprintf(disp, "Connection to %s failed.", trans);
				printf("Connection to %s failed.", trans);
		    }
		tran_in=0;
	}
	else if(connection && (movin.x !=0 || movin.y !=0))
	{
		sprintf(trans,"0 %d %d %d", movin.id, movin.x, movin.y);
		ENetPacket * packet = enet_packet_create (trans, strlen (trans) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send (peer, 0, packet);
		enet_host_flush (client);
		tran_in=0;
	}
	if(connection && tran_in)
	{
		char temp[100];
		sprintf(temp,"1 %s", trans);
		ENetPacket * packet = enet_packet_create (temp, strlen (temp) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send (peer, 0, packet);
		enet_host_flush (client);
		tran_in=0;
	}
	
	while (enet_host_service (client, & event, 10) > 0 && connection)
	{
		int n,i;
		int type;
		int a,b,c,d;
		
		char temp[10000];
		const char *ptr = temp;
		
	    switch (event.type)
	    {
	    case ENET_EVENT_TYPE_RECEIVE:
			printf("%s\n", event.packet -> data);
			sprintf(temp,"%s", event.packet -> data);
			enet_packet_destroy (event.packet);
			
			sscanf(ptr,"%d%n", &type, &n);
			ptr += n;
			interpret_packet(type, ptr);
	        break;
    
	    case ENET_EVENT_TYPE_DISCONNECT:
			strcpy(disp2,disp1);
			strcpy(disp1,disp);
	        printf ("%s disconected.\n", event.peer -> data);
			connection=0;
			sprintf (disp, "You were disconnected from the server.");
	    }
	}
}

void display(void) {
	int i,k;
	numbers();
	//-----This is the stuff involved with drawing the screen----//	
	glClearColor (0.8, 0.8,0.8,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glColor3f(0, 0, 0);
	buf[buf_num+1]=0;
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-30, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-54, GLUT_BITMAP_TIMES_ROMAN_24, trans);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-78, GLUT_BITMAP_TIMES_ROMAN_24, disp);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-78-24, GLUT_BITMAP_TIMES_ROMAN_24, disp1);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-78-24-24, GLUT_BITMAP_TIMES_ROMAN_24, disp2);
	glPopMatrix();
	
	//Display Snakes
	for(k=0; k<dispWorld.snake_num; k++)
	{
		float r=k/6.0f;
		float g=0.2f;
		float b=1-k/6.0f;
	    
		if(dispWorld.snake[k].dead==0)
		{
			glColor3f (r, g, b);
			for (i=0; i<myWorld.snake[k].size; i++) 
			{square(dispWorld.snake[k].body[i].x, dispWorld.snake[k].body[i].y);}
		}
		else if (dispWorld.snake[k].dead) 
		{
			glColor3f (r*0.5, g*0.5, b*0.5);
			for (i=0; i<wbuffer[0].snake[k].size; i++) 
			{square(dispWorld.snake[k].body[i].x, dispWorld.snake[k].body[i].y);}
		}
	}
	
	
	//Displays Food
	glColor3f (0.196078, 0.803922, 0.196078);
	for(i=0;i<dispWorld.food_num;i++)
	{
		square(dispWorld.food[i].loc.x, dispWorld.food[i].loc.y);
	}
	
	//Displays Boundaries
	glColor3f (0, 0, 0);
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f (0,scale*dispWorld.boundy, 0.0);
	glVertex3f (scale*dispWorld.boundx,scale*dispWorld.boundy, 0.0);
	glVertex3f (scale*dispWorld.boundx,scale*dispWorld.boundy, 0.0);
	glVertex3f (scale*dispWorld.boundx,0, 0.0);
    glEnd();
	glPopMatrix();
	
	glutSwapBuffers();
	
}

void pressKey(int key, int xx, int yy) {
	switch(key) {
		case GLUT_KEY_LEFT : 
			movin.x=-1;
			break;
		case GLUT_KEY_RIGHT : 
			movin.x= 1;
			break;
		case GLUT_KEY_UP : 
			movin.y= 1;
			break;
		case GLUT_KEY_DOWN : 
			movin.y=-1;
			break;
	}
}

void releaseKey(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_LEFT :
			if(movin.x<0){movin.x=0;}
			break;
		case GLUT_KEY_RIGHT : 
			if(movin.x>0){movin.x=0;}
			break;
		case GLUT_KEY_UP :
			if(movin.y>0){movin.y=0;}
			break;
		case GLUT_KEY_DOWN :
			if(movin.y<0){movin.y=0;}
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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("DonBall");
	
	init_network();
	World_init(&myWorld, 100, 100, 500, 10);
	
	glutIgnoreKeyRepeat(1);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey); 
	
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
	glutKeyboardFunc(processNormalKeys);
    glutMainLoop();
   	enet_host_destroy(client);
    return EXIT_SUCCESS;
	
}

void interpret_packet(int type, char *ptr)
{
	int n,i;		
	int tempID;		
		
	switch(type)
			{
				case 1:
					sscanf(ptr, "%d %d %d", &myWorld.boundx, &myWorld.boundy, &myWorld.food_num);
					break;
					
				case 2:
					sscanf(ptr, "%d %d", &movin.id, &myWorld.snake_num);				
					break;
					
				case 3:
					sscanf(ptr, "%d%n", &tempID, &n);
					ptr+=n;
					sscanf(ptr, 
							"%d %d %d%n", 
							&myWorld.snake[tempID].dead,
							&myWorld.snake[tempID].vel, 
							&myWorld.snake[tempID].size,
							&n);
					ptr+=n;

					for(i=0;i<myWorld.snake[tempID].size;i++){
						sscanf(ptr, 
								"%d %d%n", 
								&myWorld.snake[tempID].body[i].x, 
								&myWorld.snake[tempID].body[i].y, 
								&n);
						ptr+=n;
					}
					myWorld.snake[tempID].sizep = myWorld.snake[tempID].sizep;
					
					
					wbuffer[wbuf_num] = myWorld;
					break;
					
				case 4:
					sscanf(ptr, "%d%n", &tempID, &n);
					ptr+=n;
					
					for(i=0;i<tempID ;i++){
						sscanf(ptr, 
								"%d %d%n", 
								&myWorld.snake[i].body[0].x, 
								&myWorld.snake[i].body[0].y, 
								&n);
						//if(myWorld.snake[i].dead==0){
							//movesnake_client(&myWorld.snake[i]);
						//}
						ptr+=n;
					}
					
					if(wbuf_num < 10)
					{
						wbuffer[wbuf_num] = myWorld;
						wbuf_num++;						
					}
					
					break;
					
				case 5:
					sscanf(ptr, "%d%n", &tempID, &n);
					ptr+=n;
					sscanf(ptr, "%d", 
							&myWorld.snake[tempID].vel);
					break;
					
				case 6:
					sscanf(ptr, "%d%n", &tempID, &n);
					ptr+=n;
					sscanf(ptr, "%d", 
							&myWorld.snake[tempID].size);
					for(i=myWorld.snake[tempID].sizep; i<myWorld.snake[tempID].size; i++)
					{
						int index = myWorld.snake[tempID].sizep-1;
						myWorld.snake[tempID].body[i].x=myWorld.snake[tempID].body[index].x; 
						myWorld.snake[tempID].body[i].y=myWorld.snake[tempID].body[index].y; 
					}
					myWorld.snake[tempID].sizep=myWorld.snake[tempID].size;
					break;
					
				case 7:
					sscanf(ptr, "%d%n", &tempID, &n);
					ptr+=n;
					sscanf(ptr, "%d %d",
							&myWorld.food[tempID].loc.x, 
							&myWorld.food[tempID].loc.y);
					break;
					
				case 8:
					sscanf(ptr, "%d", &tempID);
					myWorld.snake[tempID].dead=1;
					wbuffer[wbuf_num] = myWorld;
					break;
				case 9:
					sscanf(ptr, "%d", &tempID);
					myWorld.snake[tempID].dead=0;
					wbuffer[wbuf_num] = myWorld;
					break;
				case 10:
					strcpy(disp2,disp1);
					strcpy(disp1,disp);
					strcpy(disp,ptr);
					break;
			}
}
