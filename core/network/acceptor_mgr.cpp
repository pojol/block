#include "acceptor_mgr.h"

#include "acceptor.h"

#include "err.h"

#include <algorithm>

gsf::AcceptorMgr* gsf::AcceptorMgr::instance_ = NULL;

gsf::AcceptorMgr::AcceptorMgr()
	:acceptor_index_(0)
{

}

gsf::AcceptorMgr::~AcceptorMgr()
{

}

gsf::AcceptorMgr& gsf::AcceptorMgr::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::AcceptorMgr();
	}
	return *instance_;
}

int gsf::AcceptorMgr::make_acceptor(const AcceptorConfig &config)
{
	acceptor_index_++;

	auto _acceptor_ptr = std::make_shared<Acceptor>(acceptor_index_, config);
	if (!_acceptor_ptr){
		return ACCEPTOR_CONSTRUCT_ERR;
	}

	acceptor_vec_.push_back(_acceptor_ptr);

	return acceptor_index_;
}

int gsf::AcceptorMgr::open(int acceptor_id, AcceptHandler *accept_handler)
{
	auto acceptor_ptr = find_acceptor(acceptor_id);
	if (acceptor_ptr){
		return acceptor_ptr->open(accept_handler);
	}
	else {
		return ACCEPTOR_NOT_FIND;
	}
}

int gsf::AcceptorMgr::close(int acceptor_id)
{
	auto acceptor_ptr = find_acceptor(acceptor_id);
	if (acceptor_ptr){
		return acceptor_ptr->close();
	}
	else {
		return ACCEPTOR_NOT_FIND;
	}
}

gsf::AcceptorPtr gsf::AcceptorMgr::find_acceptor(int accepor_id)
{
	auto itr = std::find_if(acceptor_vec_.begin(), acceptor_vec_.end(), [&](AcceptorVec::value_type it){
		return (it->get_id() == accepor_id);
	});
	if (itr != acceptor_vec_.end()){
		return (*itr);
	}
	else {
		return nullptr;
	}
}

