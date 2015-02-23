#ifndef __WUYA_object_POOL_H__
#define __WUYA_object_POOL_H__

#include <string>
#include <cstdlib>
#include <wuya/ipc.h>

namespace wuya {
	/**
	 * 用于缓存对象
	 * 对象类型需满足：
	 * 1. 可缺省创建
	 * 2. 有一个名为open(const P&)的方法 3.
	 * 3. 有一个名为close(const P&)的方法,并且多次调用此函数不应出现异常
	 */
	template < class T,
	class P,
	class MUTEX_TYPE, 
	class CONDITION_TYPE >
	class pool_t {
	public:
		/**
		 * initital object pool, not thread safe
		 * 
		 * @param size   size of object pool
		 * @param init_instant
		 *               是否在初始化时调用open
		 * 
		 * @return 
		 */
		pool_t();
		void init(int size=20, bool init_instant=false);
		void set_param(const P& param);
		~pool_t();
		/**
		 * to get object from pool, if have no validate object, method halt until
		 * one validate object appeared.
		 * 
		 * @return if object error, return 0
		 */
		T& get_object();
		void revert_object(T& obj);
		void close_object(T& obj);
	protected:
		T* objs_;
		int* ids_;
		bool* flags_;

		int first_available_;
		int available_;
		int size_;
		P param_;
	private:
		T& get_object_i();
		int get_id(T& obj);
	private:
		MUTEX_TYPE mutex_;
		CONDITION_TYPE condition_;
	private:
		pool_t(MUTEX_TYPE* m);
		pool_t(const pool_t& src);
		pool_t& operator=(const pool_t& src);
	};

	template < class T,
	class P,
	class MUTEX_TYPE, 
	class CONDITION_TYPE >
	class pool_proxy {
	public:
		pool_proxy(pool_t<T,P,MUTEX_TYPE,CONDITION_TYPE>& t):pool_(t) {
			obj_ = pool_.get_connect();
		}
		~pool_proxy() {
			if( obj_ ) {
				pool_.revert_connect(obj_);
				obj_=0;
			}
		}
		operator void*() {
			return(void*)obj_;
		}
		operator T&() {
			return *obj_;
		}
		T* operator->() {
			return obj_;
		}
	private:
		pool_t<T,P,MUTEX_TYPE,CONDITION_TYPE>& pool_;
		T* obj_;
	};
}

//.............................实现部分.............................//
namespace wuya {
	template <class T, class P, class M, class C>
	inline pool_t<T,P,M,C>::pool_t():size_(0), condition_(mutex_), objs_(0), ids_(0), flags_(0),
		first_available_(0), available_(0) {
	}

	template <class T, class P, class M, class C>
	inline pool_t<T,P,M,C>::~pool_t() {
		for( int i=0; i<size_;++i ) {
			objs_[i].close(param_);
		}
		delete [] objs_;
		delete [] ids_;
		delete [] flags_;
	}

	template <class T, class P, class M, class C>
	inline void pool_t<T,P,M,C>::init(int size, bool init_instant) {
		size_ = size;
		mutex_guard<M> guard(mutex_);
		guard;
		available_ = size_;
		first_available_ = 0;

		delete [] objs_;
		objs_ = new T[size];

		delete [] ids_;
		ids_ = new int[size];

		delete [] flags_;
		flags_ = new bool[size];
		
		for( int i=0; i<size; ++i ) {
			flags_[i] = false;
		}
		if( init_instant ) {
			for( int i=0; i<size; ++i ) {
				objs_[i].open(param_);
				flags_[i] = true;
			}
		}
		for( int i=0; i<available_; ++i ) {
			ids_[i] = (i+1);
		}
	}

	template <class T, class P, class M, class C>
	inline void pool_t<T,P,M,C>::set_param(const P& param){
		param_ = param;
	}
	
	template <class T, class P, class M, class C>
	inline void pool_t<T,P,M,C>::revert_object(T& obj) {
		int id = get_id(obj);
		if( id==-1 ) {
			return;
		}
		mutex_guard<M> guard(mutex_);
		guard;
		if( id<size_ ) {
			int tmp = first_available_;
			first_available_ = id;
			ids_[id] = tmp;
			++available_;
			condition_.signal();
		}
	}


	template <class T, class P, class M, class C>
	inline void pool_t<T,P,M,C>::close_object(T& obj) {
		int id = get_id(obj);
		if( id==-1 ) {
			return;
		}
		mutex_guard<M> guard(mutex_);
		guard;
		if( id<size_ ) {
			obj.close();
			int tmp = first_available_;
			first_available_ = id;
			ids_[id] = tmp;
			++available_;
			condition_.signal();
		}
	}

	template <class T, class P, class M, class C>
	inline T& pool_t<T,P,M,C>::get_object() {
		mutex_guard<M> guard(mutex_);
		guard;
		if( available_ != 0 ) {
			return get_object_i();
		} else {
			while( available_ == 0 ) {
				condition_.wait();
			}
			return get_object_i();
		}
	}

	template <class T, class P, class M, class C>
	inline int pool_t<T,P,M,C>::get_id(T& obj) {
		if( &obj-objs_<0 ) {
			return -1;
		}
		if( (&obj-objs_)%sizeof(T)==0 ) {
			return(&obj-objs_)/sizeof(T);
		}
		return -1;
	}

	template <class T, class P, class M, class C>
	inline T& pool_t<T,P,M,C>::get_object_i() {
		T& obj = objs_[first_available_];
		if( !flags_[first_available_] ) {
			obj.open(param_);
			flags_[first_available_] = true;
		}
		first_available_ = ids_[first_available_];
		--available_;
		return obj;
	}
}

#endif 

