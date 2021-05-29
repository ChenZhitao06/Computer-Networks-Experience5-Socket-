#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include <string.h>
//#include <ws2tcpip.h>
#pragma once
#pragma comment(lib,"Ws2_32.lib")

#define bzero(a, b) memset(a, 0, b)
#define bcopy(a, b,c) memcpy(b, a, c)
#define HOSTLEN 256
int make_internet_address(char* hostname, int port, struct sockaddr_in* addrp);
int make_dgram_server_socket(int portnum);
int make_dgram_client_socket();
int get_internet_address(char* host, int len, int* portp, struct sockaddr_in* addrp);
