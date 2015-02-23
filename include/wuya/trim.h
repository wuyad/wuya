#ifndef __WUYA_TRIM_H__
#define __WUYA_TRIM_H__

#include <string>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <functional>

namespace wuya{
	void tolower(std::string& str);
	void toupper(std::string& str);
	void trim_left(std::string& str);
	void trim_right(std::string& str);
	void trim(std::string& str);

	void tolower(char* str);
	void toupper(char* str);
	void trim_left(char* str);
	void trim_right(char* str);
	void trim(char* str);
}

//.............................实现部分.............................//
namespace wuya{
	inline void tolower(std::string& str) {
		for (std::string::size_type i=0; i<str.size(); ++i) {
			str[i] = std::tolower(str[i]);
		}
	}

	inline void toupper(std::string& str) {
		for (std::string::size_type i=0; i<str.size(); ++i) {
			str[i] = std::toupper(str[i]);
		}
	}

	inline void trim_left(std::string& str) {
		std::string::iterator pos = std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun( ::isspace )));
		if (pos == str.begin()) {
			return;
		} else if (pos == str.end()) {
			str.erase();
		} else {
			str.erase(str.begin(), pos);
		}
	}

	inline void trim_right(std::string& str) {
		std::string::reverse_iterator pos = std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun( ::isspace )));
		if (pos == str.rbegin()) {
			return;
		} else if (pos == str.rend()) {
			str.erase();
		} else {
			str.erase(pos.base(),  str.end() );
		}
	}

	inline void trim(std::string& str) {
		trim_right(str);
		trim_left(str);
	}

	inline void tolower(char* str) {
		size_t len = std::strlen(str);
		for (size_t i=0; i<len; ++i) {
			str[i] = std::tolower(str[i]);
		}
	}

	inline void toupper(char* str) {
		size_t len = std::strlen(str);
		for (size_t i=0; i<len; ++i) {
			str[i] = std::toupper(str[i]);
		}
	}

	inline void trim_left(char* str) {
		size_t len = std::strlen(str);
		char* pos = std::find_if(str, str+len, std::not1(std::ptr_fun(::isspace)));
		if (pos ==  str) {
			return;
		} else if (pos == str+len) {
			str[0] = '\0';
		} else {
			memmove(str, pos, std::strlen(pos)+1);
		}
	}

	inline void trim_right(char* str) {
		char* pos = str+std::strlen(str)-1;
		while (pos != str-1) {
			if (!isspace(*pos)) {
				*(pos+1) = '\0';
				break;
			}
			pos--;
		}
		if (pos == str-1)
			*(pos+1) = '\0';
	}

	inline void trim(char* str) {
		trim_left(str);
		trim_right(str);
	}
}

#endif


