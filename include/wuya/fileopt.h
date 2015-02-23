#ifndef __WUYA_FILEOPT_H__
#define __WUYA_FILEOPT_H__

#include <wuya/filestat.h>
#include <wuya/filefind.h>
#include <wuya/replacestr.h>

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#if defined(WIN32)||defined(_WIN32)
	#define FILE_SEP '\\'
	#define _WINSOCKAPI_
	#include <windows.h>
#else
	#define FILE_SEP '/'
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/stat.h>
#endif

#ifdef min
	#undef min
#endif
namespace wuya{
	void print(const char* filename);
	bool is_relative_path(const char* filename);

	template<class T> class pre_post_fix_proxy;
	template<class T> class replace_proxy;

	template<class T> class filename_only_proxy;
	template<class T> class path_only_proxy;

	template<class T>
	pre_post_fix_proxy<T> pre_post_fix(T function, const char* prefix, const char* postfix);

	template<class T>
	replace_proxy<T> replace(T function, const char* from, const char* to);

	template<class T>
	filename_only_proxy<T> filename_only(T function);

	template<class T>
	path_only_proxy<T> path_only(T function);

	class store {
	public:
		store(std::vector<std::string>& v);
		void operator()(const char* filename);
	protected:
		std::vector<std::string>& stroage_;
	};

	class count {
	public:
		count(int& nums);
		void operator()(const char* filename);
	protected:
		int& nums_;
	};

	/**
	 * 删除文件或目录，如果目录中包含文件，则删除失败
	 *
	 * @param filename 文件名或目录名
	 */
	bool remove_file(const char* filename);
	/**
	 * 删除树形目录下的所有文件及目录
	 *
	 * @param pathname 目录名
	 *
	 * @return
	 */
	bool remove_tree(const char* pathname);
	/**
	 * 复制文件至目标
	 *
	 * @param filename 源文件名
	 * @param dest_filename
	 *                 目标目录或文件名
	 *                 可以为绝对路径或是相对路径（相对于源文件所在目录）
	 */
	bool copy_file(const char* filename, const char* dest_filename, bool force=false);
	/**
	 * 移动文件
	 *
	 * @param filename 源文件名
	 * @param dest_filename
	 *                 目标目录或文件名（如果文件名中包含'*'代表源文件名）
	 *                 可以为绝对路径或是相对路径（相对于源文件所在目录）
	 */
	bool move_file(const char* filename, const char* dest_filename, bool force=false);
	/**
	 * 重命名文件
	 *
	 * @param filename   源文件名
	 * @param replace_to 替换结果，不含目录名
	 * @param replace_from
	 *                   文件名中的需替换部分，可以为"*"代表整个文件名
	 */
	bool rename_file(const char* filename, const char* replace_to, const char* replace_from="*");
	/**
	 * 创建目录
	 * 可以多级创建，如"a/b/c"
	 *
	 * @param filename 目录名
	 */
	bool make_dir(const char* filename);
}

//.............................实现部分.............................//
namespace wuya{
	template<class T>
	class pre_post_fix_proxy {
	public:
		pre_post_fix_proxy(T function, const char* prefix, const char* postfix);
		void operator()(const char* filename);
	private:
		const char* prefix_;
		const char* postfix_;
		T function_;
	};

	template <class T>
	class replace_proxy {
	public:
		replace_proxy(T function, const char* from, const char* to);
		void operator()(const char* filename);
	protected:
		const char* from_;
		const char* to_;
		T function_;
	};

	template <class T>
	class filename_only_proxy {
	public:
		filename_only_proxy(T function);
		void operator()(const char* filename);
	protected:
		T function_;
	};

	template <class T>
	class path_only_proxy {
	public:
		path_only_proxy(T function);
		void operator()(const char* filename);
	protected:
		T function_;
	};

	inline void print(const char* filename) {
		std::cout << filename << std::endl;
	}

	inline bool is_relative_path(const char* filename) {
		const char* f = filename;
		if (f[0] == FILE_SEP ||
			(f[0]=='.'&&(f[1]==FILE_SEP || f[1]=='\0'))   ||
			(f[0]=='.'&&f[1]=='.'&&(f[1]==FILE_SEP || f[1]=='\0'))
			|| (f[1]==':'&&(f[2]==FILE_SEP||f[2]=='\0'))
		   ) {
			return false;
		} else {
			return true;
		}
	}

	template<class T>
	inline pre_post_fix_proxy<T>::pre_post_fix_proxy(T function, const char* prefix,
													 const char* postfix):
	function_(function),prefix_(prefix),postfix_(postfix) {
	}

	template<class T>
	inline void pre_post_fix_proxy<T>::operator()(const char* filename) {
		std::string filename2 = prefix_;
		filename2 += filename;
		filename2 += postfix_;
		function_(filename2.c_str());
	}

	template<class T>
	inline pre_post_fix_proxy<T> pre_post_fix(T function, const char* prefix,
											  const char* postfix) {
		return pre_post_fix_proxy<T>(function, prefix, postfix);
	}

	template<class T>
	replace_proxy<T> replace(T function, const char* from, const char* to) {
		return replace_proxy<T>(function, from, to);
	}

	template<class T>
	filename_only_proxy<T> filename_only(T function) {
		return filename_only_proxy<T>(function);
	}

	template<class T>
	path_only_proxy<T> path_only(T function) {
		return path_only_proxy<T>(function);
	}

	inline store::store(std::vector<std::string>& v):stroage_(v) {
	}

	inline void store::operator()(const char* filename) {
		stroage_.push_back(filename);
	}

	inline count::count(int& nums):nums_(nums) {
	}

	inline void count::operator()(const char* filename) {
		++nums_;
	}

	template <class T>
	inline replace_proxy<T>::replace_proxy(T function, const char* from, const char* to):
	from_(from),to_(to),function_(function) {
	}

	template <class T>
	inline void replace_proxy<T>::operator()(const char* filename) {
		std::string tmp;
		replace_all(filename, from_, to_, tmp);
		function_(tmp.c_str());
	}

	template <class T>
	inline filename_only_proxy<T>::filename_only_proxy(T function):function_(function) {
	}

	template <class T>
	inline void filename_only_proxy<T>::operator()(const char* filename) {
		filestat fs(filename);
		if (fs.is_dir()) {
			return;
		}
		size_t len = strlen(filename);
		char* buf = new char[len+1];
		strcpy(buf, filename);
		char* p = buf+len;
		while (*--p!=FILE_SEP) {
			if (p<buf) {
				break;
			}
		}
		function_(p+1);
		delete [] buf;
	}

	template <class T>
	inline path_only_proxy<T>::path_only_proxy(T function):function_(function) {
	}

	template <class T>
	inline void path_only_proxy<T>::operator()(const char* filename) {
		filestat fs(filename);
		if (fs.is_dir()) {
			function_(filename);
			return;
		}
		std::string tmp=filename;
		std::string::size_type pos = tmp.find_last_of(FILE_SEP);
		if (pos != std::string::npos) {
			tmp.erase(pos);
		}
		function_(tmp.c_str());
	}

	inline bool remove_file(const char* filename) {
		filestat fs(filename);
		if (fs.exist()) {
			if (fs.is_dir()) {
#if defined(WIN32)||defined(_WIN32)
				return RemoveDirectory(filename)!=0;
#else
				return rmdir(filename)==0;
#endif
			} else {
#if defined(WIN32)||defined(_WIN32)
				SetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);
				return DeleteFile(filename)!=0;
#else
				return unlink(filename)==0;
#endif
			}
		} else {
			return false;
		}
	}

	inline void remove_tree_helper(const char* filename) {
		filestat fs(filename);
		if (fs.is_dir()) {
			remove_tree(filename);
		} else {
			remove_file(filename);
		}
	}

	inline bool remove_tree(const char* pathname) {
		filefind ff(pathname, "*", 0, filefind::inc_dir);
		ff.scan(remove_tree_helper);
		return remove_file(pathname);
	}

	inline bool copy_file(const char* filename, const char* dest_dir, bool force) {
		filestat fs_src(filename);
		if ( !fs_src.exist() || fs_src.is_dir()) {
			return false;
		}
		filestat fs_dest;
		if (is_relative_path(dest_dir)) {
			fs_dest.set(fs_src.get_filepath(), dest_dir);
			fs_dest.set(fs_dest.get_fullname());
		} else {
			fs_dest.set(dest_dir);
		}
		// 目标路径此时为绝对路径
		if (fs_dest.is_dir()) {
			fs_dest.set(fs_dest.get_filepath(), fs_src.get_filename());
		} else {
			// 目标不存在或文件
			std::size_t len = strlen(dest_dir);
			if (dest_dir[len-1] == FILE_SEP) {// 目录
				make_dir(fs_dest.get_filepath());
				fs_dest.set(fs_dest.get_filepath(), fs_src.get_filename());
			} else {// 文件
				filestat fs_tmp(fs_dest.get_filepath(), "");
				if (!fs_tmp.exist()) {
					make_dir(fs_dest.get_filepath());
				}
			}
		}
#if defined(WIN32)||defined(_WIN32)
		return ::CopyFile( filename, fs_dest.get_fullname(), !force)!=0;
#else
		const int buf_sz = std::min(fs_src.length(), 32768L);
		int infile=0, outfile=0;
		if ( (infile = ::open( filename, O_RDONLY )) < 0
			 || (outfile = ::open( fs_dest.get_fullname(),
								   force?O_RDWR | O_CREAT | O_TRUNC
								   :O_WRONLY | O_CREAT | O_EXCL,
								   S_IREAD | S_IWRITE)) < 0) {
			if ( infile >= 0 ) ::close( infile );
			return false;
		}

		char* buf = new char [buf_sz];
		int sz, sz_read=1, sz_write;
		while ( sz_read > 0
				&& (sz_read = ::read( infile, buf, buf_sz )) > 0 ) {
			sz_write = 0;
			do {
				if ( (sz = ::write( outfile, buf, sz_read - sz_write )) < 0 ) {
					sz_read = sz;
					break;
				}
				sz_write += sz;
			} while ( sz_write < sz_read );
		}
		delete [] buf;
		if ( ::close( infile) < 0 )	sz_read = -1;
		if ( ::close( outfile) < 0 ) sz_read = -1;
		std::cout << "size" << sz_read << std::endl;
		return sz_read >= 0;
#endif
	}

	inline bool move_file(const char* filename, const char* dest_dir, bool force) {
		filestat fs_src(filename);
		if ( !fs_src.exist() || fs_src.is_dir()) {
			return false;
		}
		filestat fs_dest;
		if (is_relative_path(dest_dir)) {
			fs_dest.set(fs_src.get_filepath(), dest_dir);
			fs_dest.set(fs_dest.get_fullname());
		} else {
			fs_dest.set(dest_dir);
		}
		// 目标路径此时为绝对路径
		if (fs_dest.is_dir()) {
			fs_dest.set(fs_dest.get_filepath(), fs_src.get_filename());
		} else {
			// 目标不存在或文件
			std::size_t len = strlen(dest_dir);
			if (dest_dir[len-1] == FILE_SEP) {// 目录
				make_dir(fs_dest.get_filepath());
				fs_dest.set(fs_dest.get_filepath(), fs_src.get_filename());
			} else {// 文件
				filestat fs_tmp(fs_dest.get_filepath(), "");
				if (!fs_tmp.exist()) {
					make_dir(fs_dest.get_filepath());
				}
			}
		}
		if (force && fs_dest.exist()) {
			remove_file(fs_dest.get_fullname());
		}
		return ::rename( filename, fs_dest.get_fullname())==0;
	}

	inline bool rename_file(const char* filename, const char* replace_to, const char* replace_from) {
		filestat fs(filename);
		std::string fname = fs.get_fullname();
		std::string fpath = fs.get_filepath();
		if (replace_from[0]=='*' && replace_from[1]=='\0') {
			fname = replace_to;
		} else {
			if (fs.is_dir()) {
				fname = fs.get_filepath();
				std::string::size_type len = fname.length();
				std::string::size_type pos = fname.rfind(FILE_SEP);
				if (pos != std::string::npos) {
					fpath = fname.substr(0, pos);
					fname = fname.substr(pos+1);
				}
				replace_all(fname, replace_from, replace_to);
			} else {
				fname = fs.get_filename();
				replace_all(fname, replace_from, replace_to);
			}
		}
		filestat fs2(fpath.c_str(), fname.c_str());
		return rename(filename, fs2.get_fullname())==0;
	}


#if defined(WIN32)||defined(_WIN32)
	inline bool make_dir(const char* filename) {
		std::string name = filename;
		std::string::size_type pos;
		if ( (pos=name.rfind(FILE_SEP)) != std::string::npos) {
			if (pos==0) {
				std::string tmp(filename, pos+1);
				return CreateDirectory(tmp.c_str(), 0)!=0;
			} else if (pos == name.length()-1) {
				std::string tmp(filename,0,pos);
				return make_dir(tmp.c_str());
			} else {
				std::string tmp(filename,0,pos);
				make_dir(tmp.c_str());
				tmp = name;
				return CreateDirectory(tmp.c_str(), 0)!=0;
			}
		} else {
			return CreateDirectory(filename, 0)!=0;
		}
	}
#else
	inline bool make_dir(const char* filename) {
		std::string name = filename;
		std::string::size_type pos;
		if ( (pos=name.rfind(FILE_SEP)) != std::string::npos) {
			if (pos==0) {
				std::string tmp(filename, pos+1);
				return mkdir(tmp.c_str(), S_IRWXU|S_IRWXG|S_IRWXO)==0;
			} else if (pos == name.length()-1) {
				std::string tmp(filename,0,pos);
				return make_dir(tmp.c_str());
			} else {
				std::string tmp(filename,0,pos);
				make_dir(tmp.c_str());
				tmp = name;
				return mkdir(tmp.c_str(), S_IRWXU|S_IRWXG|S_IRWXO)==0;
			}
		} else {
			return mkdir(filename, S_IRWXU|S_IRWXG|S_IRWXO)==0;
		}
	}
#endif
}

#endif

