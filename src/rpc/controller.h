/**********************************************************
 *  \file controller.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _rpc_controller_H
#define _rpc_controller_H

#include <google/protobuf/service.h>
#include "pb/net.pb.h"
#include "rpc/closure.h"
#include "rpc/frrpc_define.h"

namespace frrpc{

class Channel;	
// An RpcController mediates a single method call.  The primary purpose of
// the controller is to provide a way to manipulate settings specific to the
// RPC implementation and to find out about RPC-level errors.
//
// The methods provided by the RpcController interface are intended to be a
// "least common denominator" set of features which we expect all
// implementations to support.  Specific implementations may provide more
// advanced features (e.g. deadline propagation).
class Controller : public google::protobuf::RpcController{
	public:
		Controller();
		virtual ~Controller();
	public:
		// Client-side methods ---------------------------------------------
		// These calls may be made from the client side only.  Their results
		// are undefined on the server side (may crash).

		// Resets the RpcController to its initial state so that it may be reused in
		// a new call.  Must not be called while an RPC is in progress.
		virtual void Reset();

		// After a call has finished, returns true if the call failed.  The possible
		// reasons for failure depend on the RPC implementation.  Failed() must not
		// be called before a call has finished.  If Failed() returns true, the
		// contents of the response message are undefined.
		virtual bool Failed() const;
		// If Failed() is true, returns a human-readable description of the error.
		virtual std::string ErrorText() const;

		// Advises the RPC system that the caller desires that the RPC call be
		// canceled.  The RPC system may cancel it immediately, may wait awhile and
		// then cancel it, or may not even cancel the call at all.  If the call is
		// canceled, the "done" callback will still be called and the RpcController
		// will indicate that the call failed at that time.
		virtual void StartCancel();

		// Server-side methods ---------------------------------------------
		// These calls may be made from the server side only.  Their results
		// are undefined on the client side (may crash).

		// Causes Failed() to return true on the client side.  "reason" will be
		// incorporated into the message returned by ErrorText().  If you find
		// you need to return machine-readable information about failures, you
		// should incorporate it into your response protocol buffer and should
		// NOT call SetFailed().
		virtual void SetFailed(const std::string& reason);

		// If true, indicates that the client canceled the RPC, so the server may
		// as well give up on replying to it.  The server should still call the
		// final "done" callback.
		virtual bool IsCanceled() const;
		// Asks that the given callback be called when the RPC is canceled.  The
		// callback will always be called exactly once.  If the RPC completes without
		// being canceled, the callback will be called after completion.  If the RPC
		// has already been canceled when NotifyOnCancel() is called, the callback
		// will be called immediately.
		//
		// NotifyOnCancel() must be called no more than once per request.
		virtual void NotifyOnCancel(google::protobuf::Closure* callback);
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Controller);

		std::string error;
};

class ChannelController : public Controller{
	public:
		ChannelController(Channel* _channel, bool _is_sync);
		virtual ~ChannelController();
	public:
		inline void SetRequestId(RpcRequestId request_id){ request_id_ = request_id; }
		inline bool is_sync(){ return is_sync_; }

		virtual void Reset();

		virtual void StartCancel();
		virtual bool IsCanceled() const;
		virtual void NotifyOnCancel(google::protobuf::Closure* callback);
	private:
		bool is_cancel_;
		bool is_sync_;
		RpcRequestId request_id_;
		Channel* channel_;
		google::protobuf::Closure* callback_;
};

class ServerController : public Controller{
	public:
		ServerController();
		virtual ~ServerController();
	public:
		virtual void Reset();

		inline void set_link(LinkID link_id){ link_id_ = link_id; }
		inline LinkID link_id()const{ return link_id_; }
	private:
		LinkID link_id_;
};

}// namespace frrpc

#endif 

