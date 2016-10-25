#ifndef _TEST_SERVER_HEADER_
#define _TEST_SERVER_HEADER_


#include <map>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>

#include <stdint.h>

struct LibeventThread;

struct Conn
{
public:
	Conn(int fd);
	~Conn();

	LibeventThread *m_thread;

	const int m_fd;

	evbuffer *m_readbuf;
	evbuffer *m_writebuf;

	Conn *m_prev;
	Conn *m_next;

	LibeventThread *m_thread;
};


class ConnQueue
{
public:
	ConnQueue();
	~ConnQueue();

	Conn *push(int fd, LibeventThread *thread);
	void pop(Conn *c);
private:
	Conn *m_head;
	Conn *m_tail;
};


struct LibeventThread
{
	struct event_base *base;
	
	struct event notify_event;
	int notify_receive_fd;		//接收数据端（管道
	int notify_send_fd;			//发送数据端（管道

	ConnQueue queue;			//socket 连接链表
};


class TestServer
{
public:
	TestServer(int count);
	~TestServer();

	void setup_thread(LibeventThread *thread);

	//工作线程的入口函数
	static void * worker_libevent(void *arg);

	//主线程调度子线程的函数
	static void thread_process(int fd, short which, void *arg);

	static void listener_event_callback(evconnlistener *listener, evutil_socket_t fd
		, sockaddr *sa, int socklen, void *data);

	static void read_event_callback(struct bufferevent *bev, void *data);
	static void write_event_callback(struct bufferevent *bev, void *data);

	static void close_event_callback(struct bufferevent *bev, short events, void *data);

	bool run();

	bool add_signal_event(int sig, void(*ptr)(int, short, void*));
	bool del_signal_event(int sig);

protected:
	virtual void connection_event(Conn *conn) {}

	virtual void read_event(Conn *conn) {}

	virtual void write_event(Conn *conn) {}

	virtual void close_event(Conn *conn, short events) {}

	virtual void error_quit(const char *str);

private:
	int m_thread_count;		//worker thread number by config

	int m_port;

	LibeventThread *m_main_thread;
	LibeventThread *m_worker_threads;

	typedef std::map<int, event*> SignalEvent;
	SignalEvent m_events;
};


#endif // !_TEST_SERVER_HEADER_
