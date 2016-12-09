#ifndef _GSF_NETWORK_HEADER_
#define _GSF_NETWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>


namespace gsf
{
	namespace network
	{
		/**!
			接收器相关配置
			...
		*/
		struct AcceptorConfig;

		/**!
			连接器相关配置
		*/
		struct ConnectorConfig;

		/**!
			网络层相关配置
			worker_thread_count_ 线程数，如果不指定将会配置当前物理机核心数的线程（推荐
			send_wait_time_ 写缓冲的间隔时间，默认20ms
			read_wait_time_ 读缓冲的间隔时间，默认20ms
			update_interval_ 更新函数的间隔时间，默认20ms
		*/
		struct NetworkConfig;

		class Binder;

		//! 新的连接回调 
		typedef std::function<void(int)> NewConnectFunc;
		//! 连接失败回调
		typedef std::function<void(int, int, std::string &, int)> ConnectFailedFunc;
		//! 失去连接回调
		typedef std::function<void(int, int)> DisConnectFunc;
		//! 更新函数
		typedef std::function<void()> UpdateFunc;

		class Network
		{
		public:
			~Network();

			static Network& instance();

			//! 构建网络层
			int init(const NetworkConfig &config);

			//! 启动网络层 updatefunc 会依据配置定时出发用于实现游戏中的主循环。
			int start(UpdateFunc func);

			//! 析构网络层
			void uninit();

			//! 建立侦听器 server
			int listen(const AcceptorConfig &config, NewConnectFunc func);

			//! 建立连接器 client
			int connect(const ConnectorConfig &config, NewConnectFunc new_connect, ConnectFailedFunc connect_failed);

			//! 注册消息绑定器，一般在连接建立后调用，如果不调用此方法将接收不到注册的message事件。
			void regist_binder(Binder *binder);

			//! 向缓冲区写数据，会在周期内发送。
			void write(uint32_t session_id, const char *data, uint32_t len);

		private:
			Network();
			static Network* instance_;
		};

	}
}


#endif
