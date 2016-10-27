#include "TestServer.h"

Conn::Conn(int fd)
	: m_fd(fd)
{
	m_prev = nullptr;
	m_next = nullptr;
}

Conn::~Conn()
{}


ConnQueue::ConnQueue()
{
	m_head = new Conn(0);
	m_tail = new Conn(0);

	m_head->m_prev = m_tail->m_next = nullptr;
	m_head->m_next = m_tail;
	m_tail->m_prev = m_head;
}

ConnQueue::~ConnQueue()
{
	Conn *tcur, *tnext;
	tcur = m_head;

	while (tcur)
	{
		tnext = tcur->m_next;
		delete tcur;
		tcur = tnext;
	}
}

Conn *ConnQueue::push(int fd, LibeventThread *thread)
{
	Conn *c = new Conn(fd);
	c->m_thread = thread;
	Conn *next = m_head->m_next;

	c->m_prev = m_head;
	c->m_next = m_head->m_next;
	m_head->m_next = c;
	next->m_prev = c;

	return c;
}

void ConnQueue::pop(Conn *c)
{
	c->m_prev->m_next = c->m_next;
	c->m_next->m_prev = c->m_prev;
	delete c;
}

TestServer::TestServer(int count)
	:m_thread_count(count)
{
	m_port = -1;
	m_main_thread = new LibeventThread();
	m_worker_threads = new LibeventThread[m_thread_count];

	m_main_thread->base = event_base_new();

	for (int i = 0; i < m_thread_count; ++i)
	{
		setup_thread(&m_worker_threads[i]);
	}
}

TestServer::~TestServer()
{
	event_base_free(m_main_thread->base);

	for (int i = 0; i < m_thread_count; ++i)
	{
		event_base_free(m_worker_threads[i].base);
	}

	delete m_main_thread;
	delete[] m_worker_threads;
}

void TestServer::setup_thread(LibeventThread *thread)
{
	thread->base = event_base_new();
	if (nullptr == thread->base){
		
	}

	int fds[2];	//read,write pipe

	event_set(&thread->notify_event, thread->notify_receive_fd, EV_READ | EV_PERSIST, thread_process, thread);
	event_base_set(thread->base, &thread->notify_event);

	if (event_add(&thread->notify_event, 0) == -1){

	}
}

void *TestServer::worker_libevent(void *arg)
{
	LibeventThread *thread = static_cast<LibeventThread*>(arg);

	// ?
	event_base_dispatch(thread->base);
}

bool TestServer::run()
{

}

void TestServer::listener_event_callback(evconnlistener *listener, evutil_socket_t fd, sockaddr *sa, int socklen, void *data)
{
	TestServer *server = static_cast<TestServer*>(data);

	int num = (last_thread + 1) % m_thread_count;

	int send_fd = server->m_worker_threads[num].notify_send_fd;
	write(send_fd, &fd, sizeof(evutil_socket_t));
}

void TestServer::thread_process(int fd, short which, void *arg)
{
	LibeventThread *thread = static_cast<LibeventThread*>(arg);

	int pipe_fd = thread->notify_receive_fd;
	evutil_socket_t con_fd;
	read(pipe_fd, &con_fd, sizeof(evutil_socket_t));

	// 判断管道操作符

	struct bufferevent *bev;
	bev = bufferevent_socket_new(thread->base, con_fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev){
		return;
	}

	// 将socket连接放入队列
	Conn *conn = thread->queue.push(con_fd, thread);

	// 绑定socket 的读写事件
	bufferevent_setcb(bev, read_event_callback, write_event_callback, close_event_callback, conn);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	// 
	// call connection_event
}

void TestServer::read_event_callback(struct bufferevent *bev, void *data)
{
	Conn *conn = static_cast<Conn*>(data);

	conn->m_readbuf = bufferevent_get_input(bev);
	conn->m_writebuf = bufferevent_get_output(bev);

	//
	// call read_event
}