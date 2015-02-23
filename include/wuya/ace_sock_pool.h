#ifndef __WUYA_SOCK_POOL_H__
#define __WUYA_SOCK_POOL_H__

#include <string>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Thread_Mutex.h>
#include <ace/Condition_T.h>
#include <ace/Guard_T.h>

namespace wuya{
	/**
	 * note: must not use ACE_SOCK_Stream::close() to close the 
	 * socket, use sock_pool<T>::close() instead or 
	 * sock_pool<T>::disconnect() to delete the socket connect. 
	 */
	class sock_pool {
	public:
		enum {
			MAX_LIMIT=255
		};
		/**
		 * initital sock pool, not thread safe
		 * 
		 * @param peer_addr peer ip addr and port, such as "10.1.1.1:8080"
		 * @param timeout   connect timeout, unit: second, 0 means block
		 * @param size      size of sock pool, if size over 255, it will be
		 *                  changed to 255
		 * 
		 * @return 
		 */
		static sock_pool* init(const char* peer_addr="",int timeout=1, int size=20);
		/**
		 * get system scope singleton instance
		 */
		static sock_pool* instance();
		/**
		 * to get connect from pool, if have no validate connect, method halt until
		 * one validate connect appeared.
		 * 
		 * @param peer_addr peer ip and port, if ignore, use default ip and port from init method
		 * 
		 * @return if connect error, return 0
		 */
		ACE_SOCK_Stream* get_connect(const char* peer_addr=0);
		void close(ACE_SOCK_Stream* conn);
		void disconnect(ACE_SOCK_Stream* conn);
		/**
		 * call before system exit, you should not call it since system will call it
		 */
		static void atexit();
	protected:
		ACE_SOCK_Stream ptr_[MAX_LIMIT];
		unsigned char id_[MAX_LIMIT];
		bool connected_[MAX_LIMIT];

		unsigned char first_available_;
		unsigned char available_;
		std::string peer_addr_;
		unsigned char size_;
		int timeout_;
	private:
		ACE_SOCK_Stream* get_connect_i(const char* peer_addr=0);
		unsigned char get_id(ACE_SOCK_Stream* conn);
	private:
		ACE_Thread_Mutex mutex_;
		ACE_Thread_Condition<ACE_Thread_Mutex> condition_;
	private:
		sock_pool();
		sock_pool(const sock_pool& src);
		sock_pool& operator=(const sock_pool& src);
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline sock_pool::sock_pool():condition_(mutex_), timeout_(0), size_(20) {
	}

	inline void sock_pool::atexit() {
		sock_pool* ins = sock_pool::instance();
		if (ins == 0) {
			return;
		}
		while ((ins->available_-- > 0)) {
			ACE_SOCK_Stream& ptr = ins->ptr_[ins->first_available_];
			ins->first_available_ = ins->id_[ins->first_available_];
			ptr.close();
		}
	}

	inline sock_pool* sock_pool::init(const char* peer_addr, int timeout, int size) {
		static sock_pool* ins = 0;
		if (ins==0) {
			sock_pool* tmp = new sock_pool();
			tmp->peer_addr_ = peer_addr;
			tmp->timeout_ = timeout;
			tmp->size_ = (unsigned char)(size>MAX_LIMIT?MAX_LIMIT:size);
			tmp->available_ = tmp->size_;
			tmp->first_available_ = 0;
			for (int i=0; i<tmp->available_; ++i) {
				tmp->id_[i] = (i+1);
			}
			for (int i=0; i<tmp->available_; ++i) {
				tmp->connected_[i] = false;
			}
			::atexit(sock_pool::atexit);
			ins = tmp;
		}
		return ins;
	}

	inline sock_pool* sock_pool::instance() {
		return init();
	}

	inline void sock_pool::close(ACE_SOCK_Stream* conn) {
		unsigned char id = get_id(conn);
		ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
		guard;
		if (id<size_) {
			unsigned char tmp = first_available_;
			first_available_ = id;
			id_[id] = tmp;
			++available_;
			condition_.signal();
		}
	}

	inline void sock_pool::disconnect(ACE_SOCK_Stream* conn) {
		if (conn == 0) {
			return;
		}
		unsigned char id = get_id(conn);
		ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
		guard;
		if (id<size_) {
			conn->close();
			unsigned char tmp = first_available_;
			first_available_ = id;
			id_[id] = tmp;
			++available_;
			condition_.signal();
		}
	}

	inline ACE_SOCK_Stream* sock_pool::get_connect(const char* peer_addr) {
		ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
		guard;
		if (available_ != 0) {
			return get_connect_i(peer_addr);
		} else {
			while (available_ == 0) {
				condition_.wait();
			}
			return get_connect_i(peer_addr);
		}
	}

	inline unsigned char sock_pool::get_id(ACE_SOCK_Stream* conn) {
		return(unsigned char)(conn-ptr_);
	}

	inline ACE_SOCK_Stream* sock_pool::get_connect_i(const char* peer_addr) {
		ACE_SOCK_Stream& ptr = ptr_[first_available_];
		if (connected_[first_available_]) {
			first_available_ = id_[first_available_];
			--available_;
			return &ptr;
		} else {
			try {
				ACE_INET_Addr addr;
				if (addr.set(peer_addr) == -1) {
					return 0;
				}
				ACE_SOCK_Connector connector;
				if (timeout_ == 0) {
					if (connector.connect(ptr, addr) == -1) {
						return 0;
					}
				} else {
					ACE_Time_Value timeout(timeout_);
					if (connector.connect(ptr, addr, &timeout) == -1) {
						return 0;
					}
				}
			} catch (...) {
				return 0;
			}
			connected_[first_available_] = true;
			first_available_ = id_[first_available_];
			--available_;
			return &ptr;
		}
	}
}

#endif 


