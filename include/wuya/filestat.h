#ifndef __WUYA_FILESTAT_H__
#define __WUYA_FILESTAT_H__

#include <wuya/datetime.h>

#include <string>
#include <algorithm>
#include <sys/stat.h>
#if defined(WIN32)||defined(_WIN32)
	#pragma warning( disable : 4996)
	#include <direct.h>
	#include <io.h>
	#ifndef F_OK
		#define F_OK 00
	#endif
	#ifndef W_OK
		#define W_OK 02
	#endif
	#ifndef R_OK
		#define R_OK 04
	#endif

	#define FILE_SEP '\\'
#else
	#include <unistd.h>
	#define FILE_SEP '/'
#endif
#ifndef MAX_PATH_LEN
	#define MAX_PATH_LEN 2048
#endif


namespace wuya {
	/**
	 * 文件属性类
	 * 取文件的属性
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class filestat {
	public:
		filestat();
		// 构造函数，指定文件
		filestat(const char *filename);
		// 构造函数，指定文件
		filestat(const char *pathname, const char * filename);
	public:
		void set(const char *filename);
		void set(const char *pathname, const char * filename);
	public:
		// 取文件所处目录
		const char* get_filepath();
		// 取文件名，不含路径
		const char* get_filename();
		// 取文件全名，即含路径
		const char* get_fullname();
		// 判断是否为目录
		bool is_dir();
		// 取文件创建时间
		datetime get_create_time();
		// 取文件最后更新时间
		datetime get_last_modify_time();
		// 取文件最后访问时间
		datetime get_last_access_time();
		// 当前用户是否可读取该文件
		bool can_read();
		// 当前用户是否可写该文件
		bool can_write();
		long length();
		// 判断文件是否存在
		bool exist();
	private:
		void extract_path();
		// 文件名
		std::string  filename_;
		// 文件所处目录,除了/,d:\以外均不以FILE_SEP结尾
		std::string  path_;
		// 文件全名（含目录）
		std::string  fullname_;

		struct stat stat_;
	};
}

//.............................实现部分.............................//
namespace wuya {
	inline filestat::filestat() {
		memset(&stat_, 0, sizeof(stat_));
	}

	inline filestat::filestat(const char* filename) {
		memset(&stat_, 0, sizeof(stat_));
		set(filename);
	}

	/**
	 * pathname除了/,d:\以外均不以FILE_SEP结尾
	 * 
	 * @param filename
	 */
	inline void filestat::set(const char* filename) {
		if( filename[0]=='\0' ) {
			return;
		}
		char buf[MAX_PATH_LEN];
		memset(static_cast<void*>(&stat_), 0, sizeof(stat_));
		if( filename[0]==FILE_SEP || filename[1]==':' ) {
			// 绝对路径
			strcpy(buf, filename);
		} else {
			// 相对路径
			getcwd(buf, MAX_PATH_LEN);
			// 如果不是以FILE_SEP结尾,则须加上
			char* q = buf+strlen(buf);
			if( *(q-1) != FILE_SEP ) {
				*q++ = FILE_SEP;
				*q = 0;
			}
			strcat(buf, filename);
		}
		char* p = buf;
		char* q = buf+strlen(buf);
		// q-p>0
		if( *(q-1) == ':' ) { // C: D:
			filename_ = "";
			*q++ = FILE_SEP;
			path_.assign(p, q);
			extract_path();
			fullname_=path_;	// same as path_
			stat(fullname_.c_str(),&stat_);
			return;
		} else if( *(q-1) == FILE_SEP ) { // 以FILE_SEP结尾
			filename_ = "";
			if( p-q==1 ) { // '/'
				path_.assign(1, FILE_SEP);
				extract_path();
			} else if( *(q-2) == ':' ) {
				path_.assign(p, q);
				extract_path();
			} else {
				path_.assign(p, q-1);
				extract_path();
			}
			fullname_ = path_;
			stat(fullname_.c_str(),&stat_);
			return;
		}

		filestat fs(p, "");
		if( fs.is_dir() ) {
			path_.assign(p, q);
			extract_path();
			fullname_=path_;
			filename = "";
			stat(fullname_.c_str(),&stat_);
			return;
		}
		path_.assign(buf, q);
		extract_path();
		fullname_ = path_;
		p=q;
		while( --p>=buf && *p!=FILE_SEP );
		if( p<buf ) {// 不含有任何FILE_SEP,不可能
			return;
		}
		path_.assign(buf, p);
		extract_path();
		filename_.assign(p+1, q);
		stat(fullname_.c_str(),&stat_);
	}

	inline filestat::filestat(const char* pathname, const char * filename) {
		set(pathname, filename);
	}

	inline void filestat::set(const char* pathname, const char * filename) {
		memset(static_cast<void*>(&stat_), 0, sizeof(stat_));
		size_t len = strlen(pathname);
		filename_ = filename;
		if( pathname[len-1] != FILE_SEP ) {
			path_ = pathname;
		} else {
			path_.assign(pathname, len-1);
		}
		if( *filename==0 ) {
			fullname_ = path_;
		} else {
			fullname_ = path_ + FILE_SEP + filename_;
		}
		stat(fullname_.c_str(),&stat_);
	}


	inline const char* filestat::get_filepath() {
		return path_.c_str();
	}

	inline const char* filestat::get_fullname() {
		return fullname_.c_str();
	}

	inline const char* filestat::get_filename() {
		return filename_.c_str();
	}

	inline bool filestat::is_dir() {
#if defined(WIN32)||defined(_WIN32)
		return(stat_.st_mode&_S_IFDIR) != 0;
#else
		return  S_ISDIR(stat_.st_mode) != 0;
#endif
	}

	inline datetime filestat::get_create_time() {
		datetime ctime(stat_.st_ctime);
		return ctime;
	}

	inline datetime filestat::get_last_modify_time() {
		datetime mtime(stat_.st_mtime);
		return mtime;

	}

	inline datetime filestat::get_last_access_time() {
		datetime atime(stat_.st_atime);
		return atime;

	}

	inline bool filestat::can_read() {
		if( access(fullname_.c_str(),W_OK)!=0 )
			return false;
		else
			return true;
	}

	inline bool filestat::can_write() {
		if( access(fullname_.c_str(),R_OK)!=0 )
			return false;
		else
			return true;

	}

	inline long filestat::length() {
		return stat_.st_size;
	}

	inline bool filestat::exist() {
		if( access(fullname_.c_str(),F_OK)!=0 )
			return false;
		else
			return true;
	}

	inline void filestat::extract_path() {
		const char* begin = path_.c_str();
		const char* p = begin;
		const char* end = p+path_.length();
		const char* p0 = p;
		const char* p1 = p;
		bool match = false;
		bool one_point = false;
		bool two_point = false;

		while( p<=end ) {
			if( p==end || *p==FILE_SEP ) {
				if( match ) {
					if( two_point ) {
						path_.erase(p1-begin, p-p1);
						extract_path();
						return;
					}
					if( one_point ) {
						path_.erase(p0-begin, p-p0);
						extract_path();
						return;
					}
				}
				match = true;
				p1 = p0;
				p0 = p;
			} else if( *p == '.' ) {
				if( match ) {
					if( one_point ) {
						two_point = true;
					} else {
						one_point = true;
					}
				}
			} else {
				match = false;
				one_point = false;
				two_point = false;
			}
			++p;
		}
	}

}
#endif

