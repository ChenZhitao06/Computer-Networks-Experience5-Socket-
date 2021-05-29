/*
License server client version
link with lcIn
*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include "lcInt_funcs1.h"

void do_regular_work()
{
	printf("SuperSleep version 1.0 Running-Licensed Software\n");
	Sleep(10);
	avail_send();
}

int main(int ac, char* av[])
{
	bzero((void*)&serv_addr, sizeof(struct sockaddr_in));
	setup();
	if (get_ticket() != 0)
		exit(0);
	//while(1)
	do_regular_work();
	release_ticket();
	shut_down();
}
