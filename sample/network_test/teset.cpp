#include <stdio.h>
#include <stdint.h>

#include <WinSock2.h>

// 接口测试

class Test1 :
	public gsf::connect_handler,
	public gsf::session_close_handler
{
public :
	// 连接目标服务器成功
	virtual void handle_new_connection();

	// 连接断开
	virtual void handle_close();
};


class Test2 :
	public gsf::accept_handler,
	public gsf::session_close_handle
{
public:
	// 有新的session连接到服务器
	virtual void handle_new_connection();

	// 连接断开
	virtual void handle_close();
};


int main()
{

	return 0;
}