#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include "dgram.h"

#define oops(m,x){perror(m);exit(x);}

void say_who_called(struct sockaddr_in*);

int main(int ac, char* av[])
{
	int port;
	int sock;
	char buf[BUFSIZ];
	size_t msglen;
	struct sockaddr_in saddr;
	bzero((void*)&saddr, sizeof(struct sockaddr_in));
	int saddrlen;

	if (ac == 1 || (port = atoi(av[2])) <= 0)
	{
		fprintf(stderr, "usage:dgrecv portnumber\n");
		exit(1);
	}
	if ((sock = make_dgram_server_socket(port)) == -1)
		oops("cannot make socket", 2);
	saddrlen = sizeof(saddr);
	while ((msglen = recvfrom(sock, buf, BUFSIZ, 0, (sockaddr *)&saddr, &saddrlen)) > 0)
	{
		buf[msglen] = '\0';
		printf("dgrecv:got a message:%s\n,buf");
		say_who_called(&saddr);
	}
	return 0;
}
void say_who_called(struct sockaddr_in* addrp)
{
	char host[BUFSIZ];
	int port;
	get_internet_address(host, BUFSIZ, &port, addrp);
	printf("from:%s:%d\n", host, port);
}