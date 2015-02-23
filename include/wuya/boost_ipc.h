#ifndef __WUYA_BOOST_IPC_H__
#define __WUYA_BOOST_IPC_H__

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace wuya {
	class boost_mutex : public boost::mutex {
	public:
		void lock() {
			boost::mutex::lock();
		}
		void unlock() {
			boost::mutex::unlock();
		}
	};

	class boost_condition : public boost::condition_variable {
		public:
		boost_condition(boost_mutex& m):mutex_(m) {
		}
		void broadcast() {
			boost::condition_variable::notify_all();
		}
		void signal() {
			boost::condition_variable::notify_one();
		}
		void wait() {
			lock lk(mutex_);
			boost::condition_variable::wait(lk);
		}
	private:
		typedef boost::mutex::scoped_lock lock;
		boost_mutex& mutex_;
	};
}

#endif // __WUYA_BOOST_IPC_H__

