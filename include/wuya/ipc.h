#ifndef __WUYA_IPC_H__
#define __WUYA_IPC_H__

namespace wuya {
	class mutex_null {
	public:
		void lock(){};
		void unlock(){};
	};

	class condition_null {
	public:
        condition_null(mutex_null&){
        }
		void signal(){};
		void wait(){};
		void broadcast(){};
	};

	template<class mutex_type>
	class mutex_guard {
	public:
		mutex_guard(mutex_type& t):lock_(t) {
			lock_.lock();
		}
		~mutex_guard() {
			lock_.unlock();
		}
	private:
		mutex_type& lock_;
	};
}

//.............................实现部分.............................//
namespace wuya{
}

#endif 

