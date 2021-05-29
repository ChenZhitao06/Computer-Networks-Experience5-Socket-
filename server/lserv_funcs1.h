#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include "dgram.h"

#define SERVER_PORTNUM 2021		/*port number*/
#define MSGLEN 128				/*size of datagrams*/
#define TICKET_AVAIL 0			/*slot is available*/
#define MAXUSERS 3				
#define oops(x) {perror(x);exit(-1);}
#define AVAIL 0

int ticket_array[MAXUSERS];		/*ticket array*/
int avail_client[MAXUSERS];		/*alive client process*/
int sd = -1;					/*socket*/
int num_tickets_out = 0;		/*number of ticket out*/
int setup();
void free_all_tickets();
void shut_down();
void handle_request(char*, struct sockaddr_in*, int);
char* do_hello(char*);
char* do_goodbye(char*);
void narrate(char*, char*, struct sockaddr_in*);
char* avail_check(char* req, int a);

int setup()
{
	sd = make_dgram_server_socket(SERVER_PORTNUM);
	if (sd == -1)
		oops("make socket");
	free_all_tickets();
	return sd;
}

void free_all_tickets()
{
	int i;
	for (i = 0; i < MAXUSERS; i++)
	{
		ticket_array[i] = TICKET_AVAIL;
	}
}

void shut_down()
{
	sd = -1;		/*close(sd)*/
}

void handle_request(char* req, struct sockaddr_in* client, int addlen)
{
	char* response;
	int ret;
	if (strncmp(req, "HELO", 4) == 0)
		response = do_hello(req);
	else if (strncmp(req, "GBYE", 4) == 0)
		response = do_goodbye(req);
	else if (strncmp(req, "AVAIL", 5) == 0)
		response = avail_check(req, AVAIL);
	else
		response = "FAIL invalid request";
	narrate("SAID:", response, client);
	ret = sendto(sd, response, strlen(response), 0, (sockaddr*)client, addlen);
	if (ret == -1)
		perror("SERVER sendto failed");
}

char* do_hello(char* msg_p)
{
	int x;
	static char replybuf[MSGLEN];
	if (num_tickets_out >= MAXUSERS)
		return ("FAIL no tickets available");
	for (x = 0; x < MAXUSERS && ticket_array[x] != TICKET_AVAIL; x++)
	{
		if (x == MAXUSERS)
		{
			narrate("database corrupt", "", NULL);
			return("FAIL database corrupt");
		}
	}
	ticket_array[x] = atoi(msg_p + 5);
	sprintf(replybuf, "TICK %d.%d", ticket_array[x], x);
	num_tickets_out++;
	return(replybuf);
}

char* do_goodbye(char* msg_p)
{
	int pid, slot;
	if ((sscanf((msg_p + 5), "%d.%d", &pid, &slot) != 2) || (ticket_array[slot] != pid))
	{
		narrate("Bogus ticket", msg_p + 5, NULL);
		return("FAIL invalid ticket");
	}
	ticket_array[slot] = TICKET_AVAIL;
	num_tickets_out--;
	return("THNX See ya!");
}

void narrate(char* msg1, char* msg2, struct sockaddr_in* clientp)
{
	fprintf(stderr, "\t\tSERVER:%s %s", msg1, msg2);
	if (clientp)
		fprintf(stderr, "(%s:%d)", inet_ntoa(clientp->sin_addr), ntohs(clientp->sin_port));
	putc('\n', stderr);
}

char* avail_check(char* req,int a)
{
	static char buf[100];
	if (a == 0)
	{
		char* temp = req + sizeof(char) * 6;
		int pid = atoi(temp);
		int i;
		for (i = 0; i < MAXUSERS; i++)
		{
			if (ticket_array[i] == pid)
				break;
		}
		avail_client[i] = 0;
		sprintf(buf,"Working message is recved");
		return buf;
	}
	else 
	{
		for (int i = 0; i < MAXUSERS; i++)
		{
			if (ticket_array[i] != TICKET_AVAIL)
			{
				avail_client[i]++;
				if (avail_client[i] > 10)//10次通信后未收到进程存在的消息即判断崩溃
				{
					ticket_array[i] = TICKET_AVAIL;
					num_tickets_out--;
					avail_client[i] = 0;
					int pid = ticket_array[i];
					avail_client[i] = 0;
					printf("Process %d crash\n", pid);
					sprintf(buf, "Crash message is recved");
					return buf;
				}
			}
		}
	}
}