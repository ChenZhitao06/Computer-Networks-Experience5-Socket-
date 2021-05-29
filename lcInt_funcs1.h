/*
functions for the client of the license server
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include "dgram.h"
static int pid = 2;                     /*PID*/
static int sd = -1;                      /*Communications socket*/
struct sockaddr_in serv_addr;     /*Server address*/
static int serv_alen;					 /*length of address*/
static char ticket_buf[128];			 /*Buffer to hold ticket*/
static bool have_ticket = 0;			 /*Set when have a ticket*/

#define MSGLEN 128						 /*Size of datagrams*/
#define SERVER_PORTNUM 2021				 /*Server's port number*/
#define HOSTLEN 512
#define oops(p) {perror(p);exit(1);}
#pragma comment(lib,"Winmm.lib")

void setup();
void shut_down();
int get_ticket();
int release_ticket();
char* do_transaction(char* msg);
void narrate(char* msgl, char* msg2);
void syserr(char* msgl);
int getpid();

void setup()
{
	char hostname[BUFSIZ];
	pid = getpid();

	/*初始化*/
	WSAData wsaData1;
	sd = WSAStartup(MAKEWORD(1, 1), &wsaData1);

	sd = make_dgram_client_socket();
	if (sd == -1)
		oops("Cannot create socket");
	//printf("%d", &sd);
	gethostname(hostname, HOSTLEN);
	make_internet_address(hostname, SERVER_PORTNUM, &serv_addr);
	serv_alen = sizeof(serv_addr);
	/*char msg[2] = {'0'};
	if (sendto(sd, msg, strlen(msg), 0, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		int errNo = WSAGetLastError();
		oops("sendto failed", 3);
	}*/
}

int getpid()
{
	return GetCurrentProcessId();
}

void close(int s)
{
	sd = -1;
}

void shut_down()
{
	close(sd);
}

int get_ticket()
{
	char* response;
	char buf[MSGLEN];
	if (have_ticket)
		return(0);
	sprintf(buf, "HELO %d", pid);
	if ((response = do_transaction(buf)) == NULL)
		return -1;
	if (strncmp(response, "TICK", 4) == 0)
	{
		strcpy(ticket_buf, response + 5);
		have_ticket = 1;
		narrate("got ticket", ticket_buf);
		return 0;
	}
	if (strncmp(response, "FAIL", 4) == 0)
		narrate("Could not get ticket", response);
	else
		narrate("Unknown message:", response);
	return -1;
}

int release_ticket()
{
	char buf[MSGLEN];
	char* response;
	if (!have_ticket)
		return 0;
	sprintf(buf, "GBYE %s", ticket_buf);
	if ((response = do_transaction(buf)) == NULL)
		return -1;
	if (strncmp(response, "THNX", 4) == 0)
		narrate("released ticket OK", "");
	if (strncmp(response, "FAIL", 4) == 0)
		narrate("release failed", response + 5);
	else
		narrate("Unknown message:", response);
	return -1;
}

char* do_transaction(char* msg)
{
	static char buf[MSGLEN];
	struct sockaddr retaddr;
	int addrlen = sizeof(retaddr);
	int ret;
	ret = sendto(sd, msg, strlen(msg), 0, (sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret == SOCKET_ERROR)
	{
		int a = WSAGetLastError();
		syserr("sendto");
		return NULL;
	}
	ret = recvfrom(sd, buf, MSGLEN, 0, &retaddr, &addrlen);
	if (ret == SOCKET_ERROR)
	{
		syserr("recvfrom");
		return NULL;
	}
	return buf;
}

void narrate(char* msgl, char* msg2)
{
	fprintf(stderr, "CLIENT[%d]:%s %s\n", pid, msgl, msg2);
}

void syserr(char* msgl)
{
	char buf[MSGLEN];
	sprintf(buf, "CLIENT[%d]:%s", pid, msgl);
	perror(buf);
}

void WINAPI TimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	char buf[MSGLEN];
	sprintf(buf, "AVAIL:%d", pid);
	char* response;
	response = do_transaction(buf);
}
void avail_send()//每隔这些时间触发回调函数
{
	int n = 0;
	int timer_id = timeSetEvent(10000, 1, (LPTIMECALLBACK)TimerCallback, DWORD(1), TIME_PERIODIC);
	if (NULL == timer_id)
	{
		printf("timeSetEvent() failed with error %d\n", GetLastError());
		return ;
	}
	while (n < 20)
	{
		//printf("Hello World\n");
		Sleep(2000);
		n++;
	}
	timeKillEvent(timer_id);
}