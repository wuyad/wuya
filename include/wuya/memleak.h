#ifndef __WUYA_MEMLEAK_H__
#define __WUYA_MEMLEAK_H__

#include <new>

void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);
void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, const std::nothrow_t&) throw();
void* operator new[](size_t size, const std::nothrow_t&) throw();

void operator delete(void* pointer);
void operator delete[](void* pointer);
void operator delete(void* pointer, const char* file, int line);
void operator delete[](void* pointer, const char* file, int line);
void operator delete[](void*);
void operator delete[](void* pointer, const std::nothrow_t&);
namespace wuya{
	bool check_leaks();
}

#define new DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)

#ifdef _MSC_VER
	#define _XDEBUG_
	#undef _NEW_CRT
	#undef _DELETE_CRT
	#undef _DELETE_CRT_VEC
	#undef _STRING_CRT

	#define _NEW_CRT			new
	#define _DELETE_CRT(_P)		delete (_P)
	#define _DELETE_CRT_VEC(_P)	delete[] (_P)
	#define _STRING_CRT			string
#endif

#endif

