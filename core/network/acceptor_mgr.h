#ifndef _ACCEPTOR_MANAGER_HEADER_
#define _ACCEPTOR_MANAGER_HEADER_

#include <vector>
#include <memory>

namespace gsf
{
	struct AcceptorConfig;
	class AcceptHandler;
	class Acceptor;

	class AcceptorMgr
	{
	public:

		~AcceptorMgr();

		static AcceptorMgr & instance();

		int make_acceptor(const AcceptorConfig &config);

		int open(int acceptor_id, AcceptHandler *accept_handler);

		int close(int acceptor_id);

	protected:
		AcceptorMgr();
		static AcceptorMgr *instance_;

	private:
		typedef std::vector<std::shared_ptr<Acceptor>> AcceptorVec;
		AcceptorVec acceptor_vec_;

		uint32_t acceptor_index_;
	};
}

#endif
