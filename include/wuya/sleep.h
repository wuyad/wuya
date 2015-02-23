#ifndef __WUYA_SLEEP_H__
#define __WUYA_SLEEP_H__
#if defined(WIN32)||defined(_WIN32)
	#include <winbase.h>
#else
	#include <unistd.h>
#endif

namespace wuya{
	void sleep(unsigned int sec) {
#if defined(WIN32)||defined(_WIN32)
		::Sleep((DWORD)sec*1000);
#else
		::sleep(sec);
#endif 
	}
}
#endif 

