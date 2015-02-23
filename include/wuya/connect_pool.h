#ifndef __WUYA_CONNECT_POOL_H__
#define __WUYA_CONNECT_POOL_H__
#ifdef __HP_aCC
#define _INCLUDE_LONGLONG
#endif
#include <string>
#include <stdlib.h>

#define OTL_ORA9I
#define OTL_STREAM_READ_ITERATOR_ON
// #define OTL_UNCAUGHT_EXCEPTION_ON
#define OTL_STL // Turn on STL features
#define OTL_ORA_TIMESTAMP // enable Oracle 9i TIMESTAMPs [with [local] time zone]
//#define OTL_STREAM_POOLING_ON
#if defined(_MSC_VER) // VC++
#define OTL_BIGINT __int64
#define OTL_STR_TO_BIGINT(str,n)                \
{                                               \
  n=_atoi64(str);                               \
}

#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  _i64toa_s(n,str,sizeof(str),10);              \
}
#else
#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  _i64toa(n,str,10);                            \
}
#endif

#else

#include <stdlib.h>
#define OTL_BIGINT long long
#define OTL_STR_TO_BIGINT(str,n)                \
{                                               \
  n=strtoll(str,0,10);                          \
}
	#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  sprintf(str,"%lld",n);                        \
}

#endif

#include <otlv4.h>
#include <iostream>
#include <wuya/ipc.h>

namespace wuya {
	/**
	 * note: must not use otl_connect::close() to close the 
	 * connect, use conn_pool_t<T>::revert_connect() instead or 
	 * conn_pool_t<T>::disconnect() to delete the connect. 
	 *  
	 * normally, the maximum number of processes is about 150
	 */
	template < class mutex_type, class condition_type >
	class conn_pool_t {
	public:
		enum {
			MAX_LIMIT=255
		};
		/**
		 * initital connect pool, not thread safe
		 * 
		 * @param conn_str connect string, such as "card/card@gcoss"
		 * @param size     size of connect pool, if size over 255, it will be
		 *                 changed to 255
		 * @return 
		 */
		static conn_pool_t* init(const char* conn_str="", int size=20, bool multi_thread=false);
		/**
		 * get system scope singleton instance
		 */
		static conn_pool_t* instance();
		/**
		 * to get connect from pool, if have no validate connect, method halt until
		 * one validate connect appeared.
		 * 
		 * @return if connect error, return 0
		 */
		otl_connect* get_connect();
		void revert_connect(otl_connect* conn);
		void close_connect(otl_connect* conn);
		/**
		 * call before system exit
		 */
		static void atexit();
	protected:
		otl_connect ptr_[MAX_LIMIT];
		unsigned char id_[MAX_LIMIT];

		unsigned char first_available_;
		unsigned char available_;
		std::string conn_str_;
		unsigned char size_;
	private:
		otl_connect* get_connect_i();
		unsigned char get_id(otl_connect* conn);
	private:
		mutex_type* mutex_;
		condition_type condition_;
	private:
		conn_pool_t(mutex_type* m);
		conn_pool_t(const conn_pool_t& src);
		conn_pool_t& operator=(const conn_pool_t& src);
	};
}

//.............................实现部分.............................//
namespace wuya {
	template < class m, class c >
	inline conn_pool_t<m,c>::conn_pool_t(m* t):mutex_(t), condition_(*t) {
	}

	template < class m, class c >
	inline void conn_pool_t<m,c>::atexit() {
		conn_pool_t<m,c>* ins = conn_pool_t<m,c>::instance();
		if( ins == 0 ) {
			return;
		}
		for( int i=0; i<ins->size_;++i ) {
			ins->ptr_[i].logoff();
		}
		otl_connect::otl_terminate();
	}

	template < class m, class c >
	inline conn_pool_t<m,c>* conn_pool_t<m,c>::init(const char* conn_str, int size, 
													bool multi_thread) {
		static conn_pool_t<m,c>* ins = 0;
		static m my_mutex;
		if( ins==0 ) {
			mutex_guard<m> guard(my_mutex);
			guard;
			if( ins==0 ) {
				otl_connect::otl_initialize(multi_thread?1:0);
				conn_pool_t* tmp = new conn_pool_t<m, c>(&my_mutex);
				tmp->conn_str_ = conn_str;
				tmp->size_ = (unsigned char)(size>MAX_LIMIT?MAX_LIMIT:size);
				tmp->available_ = tmp->size_;
				tmp->first_available_ = 0;
				for( int i=0; i<tmp->available_; ++i ) {
					tmp->id_[i] = (i+1);
				}
				::atexit(conn_pool_t<m,c>::atexit);
				ins = tmp;
			}
		}
		return ins;
	}

	template < class m, class c >
	inline conn_pool_t<m,c>* conn_pool_t<m,c>::instance() {
		return init();
	}

	template < class m, class c >
	inline void conn_pool_t<m,c>::revert_connect(otl_connect* conn) {
		if( conn == 0 ) {
			return;
		}
		unsigned char id = get_id(conn);
		mutex_guard<m> guard(*mutex_);
		guard;
		if( id<size_ ) {
			unsigned char tmp = first_available_;
			first_available_ = id;
			id_[id] = tmp;
			++available_;
			condition_.signal();
		}
	}


	template < class m, class c >
	inline void conn_pool_t<m,c>::close_connect(otl_connect* conn) {
		if( conn == 0 ) {
			return;
		}
		unsigned char id = get_id(conn);
		mutex_guard<m> guard(*mutex_);
		guard;
		if( id<size_ ) {
			conn->logoff();
			unsigned char tmp = first_available_;
			first_available_ = id;
			id_[id] = tmp;
			++available_;
			condition_.signal();
		}
	}

	template < class m, class c >
	inline otl_connect* conn_pool_t<m,c>::get_connect() {
		mutex_guard<m> guard(*mutex_);
		guard;
		if( available_ != 0 ) {
			return get_connect_i();
		} else {
			while( available_ == 0 ) {
				condition_.wait();
			}
			return get_connect_i();
		}
	}

	template < class m, class c >
	inline unsigned char conn_pool_t<m,c>::get_id(otl_connect* conn) {
		return(unsigned char)(conn-ptr_);
	}

	template < class m, class c >
	inline otl_connect* conn_pool_t<m,c>::get_connect_i() {
		otl_connect& ptr = ptr_[first_available_];
		if( ptr.connected ) {
			first_available_ = id_[first_available_];
			--available_;
			return &ptr;
		} else {
			try {
				ptr.rlogon(conn_str_.c_str());
			} catch( ... ) {
				ptr.logoff();
				return 0;
			}
			first_available_ = id_[first_available_];
			--available_;
			return &ptr;
		}
	}
}

#endif 

