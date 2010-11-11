/*
 * TODO: Want to make a seperate function for each update.  Should make the code easier to understand.
 *
 * Send Packet type library
 * pt = packet type
 * cID = client's id
 * sID = snake id
 * fID = food id
 * bx, by = size (boundry) of level x and y
 * PN = peerNumber, ie number of peers connected to server
 *
 * 
 * 1 - pt bx by fn - New Game:
 * 	Sent to client when first connects gives boundries 
 * 	and food number
 * 2 - pt cID PN - Connect or Disconnect:
 * 	sent to all clients when a new client connects, 
 * 	gives client their new ID and the number of peers 
 * 	connected to the server
 * 
 *
 * 3 - pt sID dead vel size x1 y1 x2 y2 ... xsize ysize  - Snake Send:
 * 	sends the state of a snake to a connecting client
 * 
 * 4 - pt sID x y - Head Postion:
 * 	sends the postition of the head of that snake to all 	
 * 	clients
 * 5 - pt sID v - Velocity:
 * 	sends snake velocity to all clients
 * 6 - pt sID s - Size Change:
 * 	change in snake size is sent.
 * 7 - pt fID x y - Food Position:
 * 	sends new food position when it changes by being eaten
 * 8 - pt sID - Death:
 * 	sent when a snake dies
 * 9 - pt sID - start:
 * 	sent when a snake comes back to life
 * 	
 * 10- pt string - Message:
 * 	sent when a messge is sent back and forth.
 */

#include <stdio.h>
#include <string.h>
#include <enet/enet.h>
#include "world.h"

ENetAddress address;
ENetEvent event;
ENetPeer *peer[32];
ENetHost * server;
int peerNum = 0;
char message[10000];
vi2 movin;

int now;
int prev;
int check=0;
WORLD myWorld;

void send_p(int type, int sid, int cid, char *s);
void SendUpdates(void);

int main (int argc, char ** argv) 
{
	prev = clock();
	if (enet_initialize () != 0)
	{	
		fprintf (stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }
	atexit (enet_deinitialize);

	World_init(&myWorld, 100, 80, 500, 10);
	
  address.host = ENET_HOST_ANY;
  address.port = 5001;
	
	server = enet_host_create (& address /* the address to bind the server host to */, 
							   32      /* allow up to 32 clients and/or outgoing connections */,
							   2      /* allow up to 2 channels to be used, 0 and 1 */,
							   0      /* assume any amount of incoming bandwidth */,
							   0      /* assume any amount of outgoing bandwidth */);
  if (server == NULL)
  {
  	fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
    exit (EXIT_FAILURE);
  }
	
	printf("This is the Snake Server.\nPush ^c to kill the server.\n");

  /* Main simulation loop */
  while(1)
	{
		now = clock();
		check += (now - prev);
		if(check > 2000)
		{
			//	printf("%d\n",check);
			World_step(&myWorld);
			SendUpdates();
			check=0;
		}
		//send_p(5,0,0,message);
		prev = now;
		
		int i, index, temp;
		if(enet_host_service (server, & event, 10) > 0)
   		{
    		char info[100];
			switch (event.type)
    		{
    		case ENET_EVENT_TYPE_CONNECT:
    	  	printf ("A new client connected from %x: %u.\n", 
					event.peer -> address.host, 
					event.peer -> address.port);
					sprintf (info, "A new client connected from %x: %u.\n", 
					event.peer -> address.host, 
					event.peer -> address.port);
				
					peer[peerNum]=event.peer;
				
					//change world
					myWorld.snake_num++;
					i = peerNum;
					//TODO: want to replace this with a add_player function
					Snake_init(&myWorld.snake[i], 0, 5, myWorld.boundy-myWorld.boundy/myWorld.snake_num*i-myWorld.boundy/(myWorld.snake_num*2), 1, 0);
				
					//send info to client
					send_p(1,0,peerNum,info);

					for(i=0; i<peerNum+1; i++){
						send_p(3,i,peerNum,info);
					}
					for(i=0; i<myWorld.food_num; i++)
					{
						send_p(7, i, 0, info);
					}
					peerNum++;
				
					send_p(2,0,0,info);
					send_p(10,0,0,info);
					break;

    	 	case ENET_EVENT_TYPE_RECEIVE:
					sprintf(info, "%s", event.packet -> data);
					enet_packet_destroy (event.packet);
				
					sscanf(info, "%d", &temp);
					if(temp==0)
					{
						//TODO: don't want to trust the client here.  search for the id it came from and use that.
						sscanf(info, "%d %d %d %d", &temp, &movin.id, &movin.x, &movin.y);
						myWorld.snake[movin.id].movin.x = movin.x;
						myWorld.snake[movin.id].movin.y = movin.y;
					}
					else if(temp==1){
						printf(info);
						send_p(10,0,0,info);
					}
    	    break;
    	
    	  case ENET_EVENT_TYPE_DISCONNECT:		
					printf ("A client disconnected from %x: %u.\n", 
							event.peer -> address.host, 
							event.peer -> address.port);
					sprintf (info, "A client disconnected from %x: %u.\n", 
						event.peer -> address.host, 
						event.peer -> address.port);
				
    	
					/*Move the people down so there is the correct number of clients*/
					for(i=0; i<peerNum; i++)
					{
						if(peer[i]==event.peer)
						{
							index=i;
							i=peerNum;
						}
					}

					/**should do a remove player function here.**/
					for(i=index; i<peerNum; i++)
					{
						peer[i]=peer[i+1];
						myWorld.snake[i]=myWorld.snake[i+1];
					}
				
					peerNum--;
					myWorld.snake_num--;
    	    event.peer -> data = NULL;
					send_p(2,0,0,info);
					send_p(10,0,0,info);
					break;
    	}
    }
	}
    
	enet_host_destroy(server);
}


void send_p(int type, int sid, int cid, char *s)
{
	int i;
	char temp[10];
	ENetPacket * packet;
	switch(type)
	{
		case 1:
			sprintf(message, "1 %d %d %d", myWorld.boundx, myWorld.boundy, myWorld.food_num);
			
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send (peer[cid], 0, packet);
			break;
		case 2:
			for(i=0; i<peerNum; i++)
			{
				sprintf(message, "2 %d %d", i, peerNum);		
				packet = enet_packet_create (message, strlen (message) + 1,ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send (peer[i], 0, packet);
			}
			break;
		case 3:
			sprintf(message, 
					"3 %d %d %d %d", 
					sid, 
					myWorld.snake[sid].dead,
					myWorld.snake[sid].vel, 
					myWorld.snake[sid].size);

			for(i=0;i<myWorld.snake[sid].size;i++){
				sprintf(temp, " %d %d", myWorld.snake[sid].body[i].x, myWorld.snake[sid].body[i].y);
				strcat(message, temp);
			}
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		
		case 4:
			sprintf(message, 
					"4 %d ", 
					myWorld.snake_num);

			for(i=0;i<myWorld.snake_num;i++){
				sprintf(temp, "%d %d ", myWorld.snake[i].body[0].x, myWorld.snake[i].body[0].y);
				strcat(message, temp);
			}
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		case 5:
			sprintf(message, "5 %d %d",
					sid, 
					myWorld.snake[sid].vel);
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		case 6:
			sprintf(message, "6 %d %d",
					sid, 
					myWorld.snake[sid].size);
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		case 7:
			sprintf(message, "7 %d %d %d",
					sid, 
					myWorld.food[sid].loc.x, 
					myWorld.food[sid].loc.y);
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		case 8:
			sprintf(message, "8 %d", sid);
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		case 9:
			sprintf(message, "9 %d", sid);
			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
		case 10:
			sprintf(message, "10 %s", s);

			packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (server, 0, packet);
			break;
	}
}

void SendUpdates(void)
{
	int i;
	for(i=0; i<myWorld.food_num; i++)
	{
		if(myWorld.food[i].send)
		{
			send_p(7,i,0,message);
			myWorld.food[i].send=0;
		}
	}
	
	for(i=0; i<myWorld.snake_num; i++)
	{
		send_p(3,i,0,message);
	/*	if(myWorld.snake[i].dead==0){
			send_p(4,i,0,message);
		}
		
		if(myWorld.snake[i].send_vel)
		{
		//	send_p(5,i,0,message);
			myWorld.snake[i].send_vel=0;
		}
		if(myWorld.snake[i].send_dead)
		{
			send_p(8,i,0,message);
			myWorld.snake[i].send_dead=0;
		}
		if(myWorld.snake[i].send_start)
		{
			send_p(9,i,0,message);
			myWorld.snake[i].send_start=0;
		}
		if(myWorld.snake[i].send_size)
		{
			send_p(6,i,0,message);
			myWorld.snake[i].send_size=0;
		}*/
	}
}
