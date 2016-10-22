#include <stdio.h>
#include <stdint.h>

#include <WinSock2.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>


void read_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer *input;
	char *request_line;
	size_t len;

	input = bufferevent_get_input(bev);

	size_t output_len = evbuffer_get_length(input);

	while (true)
	{
		request_line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF);
		if (NULL == request_line){
			free(request_line);
		}
		else {
			printf("get one line data : %s\n", request_line);
			if (strstr(request_line, "over") != NULL){
				free(request_line);
				break;
			}
		}

		free(request_line);
	}
}

void write_cb(struct bufferevent *bev, void *ctx)
{

}

void error_cb(struct bufferevent *bev, short error, void *ctx)
{
	if (error & BEV_EVENT_EOF)
	{
		/* connection has been closed, do any clean up here */
		printf("connection closed\n");
	}
	else if (error & BEV_EVENT_ERROR)
	{
		/* check errno to see what error occurred */
		printf("some other error\n");
	}
	else if (error & BEV_EVENT_TIMEOUT)
	{
		/* must be a timeout event handle, handle it */
		printf("Timed out\n");
	}
	bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
	struct event_base *base = (struct event_base*)arg;
	struct sockaddr addr;

	int len = sizeof(addr);
	int fd = accept(listener, (struct sockaddr*)&addr, &len);
	if (fd < 0){
		
	}
	else if (fd > FD_SETSIZE){
		
	}
	else {
		struct bufferevent *bev;
		evutil_make_socket_nonblocking(fd);

		bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

		bufferevent_setcb(bev, read_cb, write_cb, error_cb, NULL);
		bufferevent_enable(bev, EV_READ | EV_WRITE);
	}

}


int main()
{
	setvbuf(stdout, NULL, _IONBF, 0);

#ifdef WIN32
	//Æô¶¯socket
#endif // WIN32

	evutil_socket_t listener;

	struct sockaddr_in sin;
	struct event_base *base;
	struct event *listener_event;

	base = event_base_new();
	if (!base){
		return 1;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(7777);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	evutil_make_socket_nonblocking(listener);

#ifndef WIN32
	int one = 1;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#endif

	if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		return 1;
	}

	if (listen(listener, 16) < 0){
		return 1;
	}

	listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)base);

	event_add(listener_event, NULL);	//check it ?

	event_base_dispatch(base);

	return 0;
}