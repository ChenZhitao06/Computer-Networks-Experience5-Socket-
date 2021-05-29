#include "dgram.h"

int make_dgram_server_socket(int portnum)
{
	struct sockaddr_in saddr;
	char hostname[HOSTLEN];
	int sock_id;
	WSAData wsaData1;
	sock_id = WSAStartup(MAKEWORD(1, 1), &wsaData1);
	sock_id = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock_id == -1) return -1;
	gethostname(hostname, HOSTLEN);
	make_internet_address(hostname, portnum, &saddr);
	if (bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
		return -1;
	return sock_id;
}

int make_dgram_client_socket()
{
	return socket(PF_INET, SOCK_DGRAM, 0);
}

int make_internet_address(char* hostname, int port, struct sockaddr_in* addrp)
{
	struct hostent* hp;
	bzero((void*)addrp, sizeof(struct sockaddr_in));
	hp = (hostent*)gethostbyname(hostname);
	if (hp == NULL) return -1;
	bcopy((void*)addrp, (void*)&addrp->sin_addr, hp->h_length);
	addrp->sin_port = htons(port);
	addrp->sin_family = AF_INET;
	return 0;
}

int get_internet_address(char* host, int len, int* portp, struct sockaddr_in* addrp)
{
	strncpy(host, inet_ntoa(addrp->sin_addr), len);
	*portp = ntohs(addrp->sin_port);
	return 0;
}
