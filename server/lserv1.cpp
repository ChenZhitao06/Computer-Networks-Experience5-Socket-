#include "lserv_funcs1.h"

int main(int sc, char* av[])
{
	struct sockaddr_in client_addr;
	bzero((void*)&client_addr, sizeof(struct sockaddr_in));
	int addrlen = sizeof(client_addr);
	char buf[MSGLEN];
	int ret;
	int sock;
	sock = setup();
	while (1)
	{
		addrlen = sizeof(client_addr);
		ret = recvfrom(sock, buf, MSGLEN, 0, (sockaddr*)&client_addr, &addrlen);
		if (ret != -1)
		{
			buf[ret] = '\0';
			narrate("GOT", buf, &client_addr);
			handle_request(buf, &client_addr, addrlen);
		}
		else if (errno != EINTR)
			perror("recvfrom");
		avail_check(buf, 1);
	}
}