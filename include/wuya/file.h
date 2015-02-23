#ifndef __WUYA_FILE_H__
#define __WUYA_FILE_H__

#include <cstdio>
#include <string>

namespace wuya{
	/**
	 * �ļ���д��
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class file {
	public:
		// �����ļ���λ��ʶ
		enum seek_position {
			begin = SEEK_SET, current = SEEK_CUR, end = SEEK_END
		};
		file();
		// ���캯���������Ѿ�ͨ��fopen�򿪵��ļ����
		file(FILE* handle);
		// ���캯��������ָ��ģʽ���ļ�����������ͬfopen
		file(const char *filename, const char *mode);
		~file();
	public:
		// ����ָ��ģʽ���ļ�����������ͬfopen
		bool open(const char *filename, const char *mode);
		// ��ȡָ�������������ڴ�
		size_t read(void* buf, size_t count);
		// дָ�������������ļ�
		size_t write(const void* buf, size_t count);
		// д�ַ������ļ�
		bool write_string(const char* buf, bool newline = true);
		// ��ȡһ��
		std::string read_string();
		// ��λ�ļ�ָ��
		long seek(long offset, seek_position from = begin);
		// ���ļ�������
		void flush();
		// �ر��ļ�
		void close();
		// ��һ��
		bool read_string(std::string& str);
		// ��һ��
		char* read_string(char* buf, unsigned int max_size);
		// ��λ�ļ�ָ�����ļ���ʼ
		long seek_to_end();
		// ��λ�ļ�ָ�����ļ���β
		void seek_to_begin();
	public:
		// �ж��Ƿ����ļ���β
		bool eof();
		// ȡ�ļ�����
		long length() const;
		// ���ص�ǰ�ļ�ָ��λ��
		long get_pos() const;
	protected:
		// �ļ����
		FILE* handle_;
	};
}

//.............................ʵ�ֲ���.............................//
namespace wuya{
	inline file::file():handle_(0) {
	}

	inline file::file(FILE* handle) {
		handle_ = handle;
	}

	inline file::file(const char *filename, const char *mode) {
		open(filename, mode );
	}


	inline file::~file() {
		if ( handle_ != 0 ) {
			flush();
			close();
		}
	}

	inline bool file::open(const char *filename, const char *mode) {
		return(handle_ = fopen(filename, mode)) != 0;
	}

	inline size_t file::read(void* buf, size_t count) {
		if ( handle_ == 0 )	return 0;
		if ( count == 0 )
			return 0;
		size_t  nread = fread(buf, 1, count, handle_);
		if ( ferror(handle_) ) {
			clearerr(handle_);
			nread = 0;
		}
		return nread;
	}


	inline size_t file::write(const void* buf, size_t count) {
		if ( handle_ == 0 )	return 0;
		return fwrite(buf, sizeof(char), count, handle_);
	}

	inline bool file::write_string(const char* buf, bool newline) {
		if ( handle_ == 0 )	return false;
		int ret = 0;

		if ( (ret = fputs(buf, handle_)) == EOF ) {
			return false;
		}

		if ( newline && (ret = fputc('\n', handle_)) == EOF ) {
			return false;
		}
		return true;
	}

	inline std::string file::read_string() {
		if ( handle_ == 0 )	return "";
		std::string s = "";
		read_string(s);
		return s;
	}

	inline long file::seek(long offset, seek_position from) {
		if ( handle_ == 0 )	return -1;
		if ( fseek(handle_, offset, from)!= 0 ) {
			return -1;
		} else {
			long pos = ftell(handle_);
			return pos;
		}
	}

	inline long file::get_pos() const {
		if ( handle_ == 0 )	return -1;
		return  ftell(handle_);
	}

	inline void file::flush() {
		if ( handle_ == 0 )	return;
		fflush(handle_);
	}

	inline void file::close() {
		if ( handle_ == 0 )	return;
		fclose(handle_);
		handle_ = 0;
	}

	inline char* file::read_string(char* buf, unsigned int max_size) {
		if ( handle_ == 0 )	return 0;

		char* b = fgets(buf, max_size, handle_);
		if ( b == 0 && !feof(handle_) ) {
			clearerr(handle_);
			return 0;
		}
		size_t len = strlen(buf);
		if ( len>0 && buf[len-1]=='\n' )
			buf[len-1] = '\0';
		if ( len>1 && buf[len-2]=='\r' )
			buf[len-2] = '\0';

		return buf;
	}

	inline bool file::read_string(std::string& str) {
		if ( handle_ == 0 )	return false;
		str = "";
		const int max_sizeSize = 128;
		char buf[max_sizeSize+1];
		char* b;
		size_t len = 0;
		for ( ;; ) {
			buf[0] = '\0';
			b = fgets(buf, max_sizeSize+1, handle_);

			// handle error/eof case
			if ( b == 0 && !feof(handle_) ) {
				clearerr(handle_);
				return false;
			}

			// if string is read completely or eof
			str += buf;
			if ( b == 0 ||
				 (len = strlen(buf)) < max_sizeSize ||
				 buf[len-1] == '\n' ) {
				break;
			}
		}

		// remove '\n' from end of string if present
		len = str.size();
		if ( len > 0 && str[len-1] == '\n' )
			str.erase(len-1);
		if ( len > 1 && str[len-2] == '\r' )
			str.erase(len-2);
		return b != 0;
	}

	inline long file::length() const {
		if ( handle_ == 0 )	return -1;
		long cur;
		long len;
		long result;

		cur = ftell(handle_);
		if (cur == -1) {
			return -1;
		}

		result = fseek(handle_, 0, SEEK_END);
		if (result != 0) {
			return -1;
		}

		len = ftell(handle_);
		if (len == -1) {
			return -1;
		}
		result = fseek(handle_, cur, SEEK_SET);
		if (result != 0) {
			return -1;
		}

		return len;
	}

	inline long file::seek_to_end() {
		if ( handle_ == 0 )	return -1;
		if ( fseek(handle_,0l, SEEK_END) ) {
			return -1;//faild
		} else {
			return(this->get_pos());
		}
	}

	inline void file::seek_to_begin() {
		if ( handle_ == 0 )	return;
		fseek(handle_,0l,SEEK_SET);
	}

	inline bool file::eof() {
		if ( handle_ == 0 )	return false;
		return feof(handle_) != 0;
	}
}

#endif

