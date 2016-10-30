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

#include <io.h>


void singal_cb(evutil_socket_t fd, short event, void * arg)
{

	char buf[1];
	int n = recv(fd, (char*)buf, sizeof(buf), 0);
	if (n == -1){
		int err = GetLastError();
	}
	else if (n == 0){
	}

	printf("hello\n");
}

int main()
{
	struct event_base *base;

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0){
		exit(1);
	}
#endif // WIN32

	base = event_base_new();

	evutil_socket_t pipe[2];
	if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, pipe) < 0){
		
	}
	evutil_make_socket_nonblocking(pipe[0]);
	evutil_make_socket_nonblocking(pipe[1]);


	struct event *signal;
	signal = event_new(base, pipe[0], EV_WRITE | EV_PERSIST, singal_cb, NULL);
	event_add(signal, NULL);
	

	char buf[1];
	buf[0] = 'c';
	send(pipe[0], buf, 1, 0);

	

	event_base_dispatch(base);
	event_base_free(base);

	return 0;
}