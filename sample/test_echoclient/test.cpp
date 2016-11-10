#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>

#include <event2/event-config.h>
#include <event2/event.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif // WIN32

#include <network.h>
#include <acceptor.h>
#include <accept_handler.h>

#include <iostream>

//! user interface test

int main()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0){
		exit(1);
	}
#endif // WIN32

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	int ret;
	ret = connect(socket_fd, (sockaddr*)&addr, sizeof(sockaddr));

	ret = send(socket_fd, "1", 1, 0);

	char buf[1024];
	recv(socket_fd, buf, 1024, 0);
	std::cout << buf << std::endl;

	system("pause");
	return 0;
}
