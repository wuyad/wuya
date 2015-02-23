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
	 * �ļ�������
	 * ȡ�ļ�������
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class filestat {
	public:
		filestat();
		// ���캯����ָ���ļ�
		filestat(const char *filename);
		// ���캯����ָ���ļ�
		filestat(const char *pathname, const char * filename);
	public:
		void set(const char *filename);
		void set(const char *pathname, const char * filename);
	public:
		// ȡ�ļ�����Ŀ¼
		const char* get_filepath();
		// ȡ�ļ���������·��
		const char* get_filename();
		// ȡ�ļ�ȫ��������·��
		const char* get_fullname();
		// �ж��Ƿ�ΪĿ¼
		bool is_dir();
		// ȡ�ļ�����ʱ��
		datetime get_create_time();
		// ȡ�ļ�������ʱ��
		datetime get_last_modify_time();
		// ȡ�ļ�������ʱ��
		datetime get_last_access_time();
		// ��ǰ�û��Ƿ�ɶ�ȡ���ļ�
		bool can_read();
		// ��ǰ�û��Ƿ��д���ļ�
		bool can_write();
		long length();
		// �ж��ļ��Ƿ����
		bool exist();
	private:
		void extract_path();
		// �ļ���
		std::string  filename_;
		// �ļ�����Ŀ¼,����/,d:\���������FILE_SEP��β
		std::string  path_;
		// �ļ�ȫ������Ŀ¼��
		std::string  fullname_;

		struct stat stat_;
	};
}

//.............................ʵ�ֲ���.............................//
namespace wuya {
	inline filestat::filestat() {
		memset(&stat_, 0, sizeof(stat_));
	}

	inline filestat::filestat(const char* filename) {
		memset(&stat_, 0, sizeof(stat_));
		set(filename);
	}

	/**
	 * pathname����/,d:\���������FILE_SEP��β
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
			// ����·��
			strcpy(buf, filename);
		} else {
			// ���·��
			getcwd(buf, MAX_PATH_LEN);
			// ���������FILE_SEP��β,�������
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
		} else if( *(q-1) == FILE_SEP ) { // ��FILE_SEP��β
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
		if( p<buf ) {// �������κ�FILE_SEP,������
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

