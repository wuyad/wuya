#ifndef __WUYA_ICONV_H__
#define __WUYA_ICONV_H__

#include <iconv.h>
#include <errno.h>
#include <string>
namespace wuya{
	/**
	 * 转换不同的编码，如ASCII,UTF-8,GBK,UCS-2,UTF-16
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class iconv {
	public:
		iconv();
		iconv( const char* fromcode, const char* tocode, int buffer_size=64);
		~iconv();
	public:
		bool open( const char* fromcode, const char* tocode, int buffer_size=64);
		std::string convert(const std::string& in);
	private:
		iconv_t handle_;
		int buffer_size_;
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline iconv::iconv():handle_((iconv_t)(-1)), buffer_size_(64) {
	}
	inline iconv::iconv(const char* fromcode, const char* tocode, int buffer_size):handle_((iconv_t)(-1)) {
		open(fromcode, tocode,buffer_size);
	}
	inline iconv::~iconv() {
		if (handle_ != (iconv_t)(-1)) {
			iconv_close(handle_);
		}
	}
	inline bool iconv::open(const char* fromcode, const char* tocode, int buffer_size) {
		buffer_size_ = buffer_size;
		handle_ = iconv_open(tocode, fromcode);
		return(handle_!=(iconv_t)-1);
	}
	inline std::string iconv::convert(const std::string& in) {
		using std::string;
		size_t inleft=in.length(), outleft=buffer_size_;
		char* buf = new char[buffer_size_];
		char* out_buf = buf;
		const char* src = in.c_str();
		string ret="";
		while (1) {
			size_t l = ::iconv(handle_, (char**)&src, &inleft, &out_buf, &outleft);
			if (l != (size_t)-1) {
				ret.append(buf, buffer_size_-outleft);
				if (inleft == 0) {
					break;
				}
			} else if (errno == E2BIG) {
				ret.append(buf, buffer_size_-outleft);
				outleft = buffer_size_;
				out_buf = buf;
				errno = 0;
			} else {
				ret = "";
				int errn = errno;
				int errn2 = errno;
				break;
			}
		}
		delete [] buf;
		return ret;
	}
}


#endif

