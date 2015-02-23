#ifndef __WUYA_ACE_IPC_H__
#define __WUYA_ACE_IPC_H__

#include <ace/Thread_Mutex.h>
#include <ace/Condition_T.h>

namespace wuya {
	class ace_mutex : public ACE_Thread_Mutex {
	public:
		void lock() {
			ACE_Thread_Mutex::acquire();
		}
		void unlock() {
			ACE_Thread_Mutex::release();
		}
	};

	class ace_condition : public ACE_Condition<ACE_Thread_Mutex> {
	public:
		ace_condition(ace_mutex& m):ACE_Condition<ACE_Thread_Mutex>(m) {
		}
		void broadcast() {
			ACE_Condition<ACE_Thread_Mutex>::broadcast();
		}
		void signal() {
			ACE_Condition<ACE_Thread_Mutex>::signal();
		}
		void wait() {
			ACE_Condition<ACE_Thread_Mutex>::wait();
		}
	};
}

#endif // __WUYA_ACE_IPC_H__

