#ifndef __WUYA_FILEFINE_H__
#define __WUYA_FILEFINE_H__

#include <wuya/wildcard.h>
#include <wuya/filestat.h>
#if defined(WIN32)||defined(_WIN32)
	#define _WINSOCKAPI_
	#include <windows.h>
#else
	#include <dirent.h>
#endif
#include <cstring>
#include <string>

namespace wuya{
	/**
	 * 文件搜索类
	 * 搜索指定目录下的文件，可指定深度
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class filefind {
	public:
		enum dir_flag {
			/**
			 * 0-结果不包含目录
			 * 1-结果包含目录，且先于子目录返回
			 * 2-结果包含目录，且其子目录先返回
			 * 3-结果只包含目录，且先于子目录返回
			 * 4-结果只包含目录，且其子目录先返回
			 */
			no_dir,inc_dir,inc_dir_subfirst, only_dir, only_dir_subfirst
		};
		filefind();
		/**
		 * 构造函数，指定待搜索目录并打开目录
		 *
		 * @param dirname 目录名
		 * @param matcher 匹配子，可多重匹配，以';'隔开，如"*.cpp;*.h"
		 * @param depth   深度，depth=0意指仅搜索当前目录
		 * @param flag    是否包含目录，且与其子目录的先后关系
		 */
		filefind(const char *dirname, const char* matcher="*", unsigned depth=0, dir_flag flag=no_dir);
	public:
		void set(const char *dirname, const char* matcher="*", unsigned depth=0, dir_flag flag=no_dir);
		/**
		 * 取下一个文件/目录，结束时返回0
		 *
		 * @param T        形如void functor(const char* filename)的函数指针或函数对象
		 * @param function 回调函数
		 */
		template<class T>
		void scan(T function);
	protected:
		// 目录名
		std::string dir_;
		const char* matcher_;
		unsigned int depth_;
		dir_flag flag_;

#if defined(WIN32)||defined(_WIN32)
		HANDLE handler_;
		WIN32_FIND_DATA tmp_data_;
#define FILE_SEP '\\'
		template<class T>
		void scan_impl_2(const std::string& sub_dir, unsigned depth, bool reopen, T& func);
#else
		DIR* handler_;
		struct dirent tmp_data_;
#define FILE_SEP '/'
#define INVALID_HANDLE_VALUE 0
#endif
	private:
		template<class T>
		void scan_impl(const std::string& sub_dir, unsigned depth, bool reopen, T& func);

		void merge_path(const char* dir, const char* sub_dir);
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline filefind::filefind():dir_("."),matcher_("*") {
	}

	inline filefind::filefind(const char *dirname, const char* matcher, unsigned depth,
							  dir_flag flag):matcher_(matcher),depth_(depth),dir_(dirname),
	flag_(flag), handler_(INVALID_HANDLE_VALUE) {
		if (dir_[dir_.length()-1] != FILE_SEP) {
			dir_ += FILE_SEP;
		}
	}

	inline void filefind::set(const char *dirname, const char* matcher, unsigned depth,
							  dir_flag flag) {
		matcher_ = matcher;
		depth_ = depth;
		dir_ = dirname;
		if (dir_[dir_.length()-1] != FILE_SEP) {
			dir_ += FILE_SEP;
		}
		handler_ = INVALID_HANDLE_VALUE;
        flag_ = flag;
	}

	template<class T>
	inline void filefind::scan(T func) {
		return scan_impl(dir_, 0, false, func);
	}

#if defined(WIN32)||defined(_WIN32)
	template<class T>
	inline void filefind::scan_impl(const std::string& sub_dir, unsigned depth,
									bool reopen, T& func) {
		if (reopen || handler_ == INVALID_HANDLE_VALUE) {
			handler_=FindFirstFile((sub_dir+"*").c_str(), &tmp_data_);
			if (handler_ == INVALID_HANDLE_VALUE) {
				return;
			}
			scan_impl_2(sub_dir, depth, reopen, func);
		}
		while (FindNextFile(handler_, &tmp_data_) != 0) {
			scan_impl_2(sub_dir, depth, reopen, func);
		}
		FindClose(handler_);
		handler_ = INVALID_HANDLE_VALUE;
	}

	template<class T>
	inline void filefind::scan_impl_2(const std::string& sub_dir, unsigned depth,
									  bool reopen, T& func) {
		if (strcmp(tmp_data_.cFileName, ".") == 0 || strcmp(tmp_data_.cFileName, "..") == 0) {
			return;
		}
		bool m = multimatch(tmp_data_.cFileName, matcher_, 0, true, ';');

		filestat fst(sub_dir.c_str(), tmp_data_.cFileName);
		if (fst.is_dir()) {
			if (depth < depth_) {
				if ((flag_ == inc_dir || flag_ == only_dir) && m) {
					merge_path(sub_dir.c_str(), tmp_data_.cFileName);
					func(tmp_data_.cFileName);
				}
				HANDLE handler = handler_;
				std::string last_sub_dir = sub_dir+tmp_data_.cFileName;
				scan_impl(std::string(fst.get_fullname())+FILE_SEP, depth+1, true, func);
				handler_ = handler;
				if ((flag_ == inc_dir_subfirst || flag_ == only_dir_subfirst) && m) {
					func(last_sub_dir.c_str());
				}
			} else {
				// 最后一层
				if (flag_ != 0 && m) {
					merge_path(sub_dir.c_str(), tmp_data_.cFileName);
					func(tmp_data_.cFileName);
				}
			}
		} else {
			if (flag_ == only_dir || flag_ == only_dir_subfirst) {
				return;
			}
			if (m) {
				// 普通文件
				merge_path(sub_dir.c_str(), tmp_data_.cFileName);
				func(tmp_data_.cFileName);
			}
		}
	}
#else
	template<class T>
	inline void filefind::scan_impl(const std::string& sub_dir, unsigned depth,
									bool reopen, T& func) {
		if (reopen || handler_ == INVALID_HANDLE_VALUE) {
			handler_=opendir(sub_dir.c_str());
			if (handler_ == INVALID_HANDLE_VALUE) {
				return;
			}
		}

		struct dirent * result;
		while (readdir_r(handler_, &tmp_data_, &result)==0) {
			if(result==0) {
				break;
			}
			if (strcmp(tmp_data_.d_name, ".") == 0 || strcmp(tmp_data_.d_name, "..") == 0) {
				continue;
			}
			bool m = multimatch(tmp_data_.d_name, matcher_, 0, true, ';');

			filestat fst(sub_dir.c_str(), tmp_data_.d_name);
			if (fst.is_dir()) {
				if (depth < depth_) {
					if ((flag_ == inc_dir || flag_ == only_dir) && m) {
						merge_path(sub_dir.c_str(), tmp_data_.d_name);
						func(tmp_data_.d_name);
					}
					DIR* handler = handler_;
					std::string last_sub_dir = sub_dir+tmp_data_.d_name;
					scan_impl(std::string(fst.get_fullname())+FILE_SEP, depth+1, true, func);
					handler_ = handler;
					if ((flag_ == inc_dir_subfirst || flag_ == only_dir_subfirst) && m) {
						func(last_sub_dir.c_str());
					}
				} else {
					// 最后一层
					if (flag_ != 0 && m) {
						merge_path(sub_dir.c_str(), tmp_data_.d_name);
						func(tmp_data_.d_name);
					}
				}
			} else {
				if (flag_ == only_dir || flag_ == only_dir_subfirst) {
					continue;
				}
				if (m) {
					// 普通文件
					merge_path(sub_dir.c_str(), tmp_data_.d_name);
					func(tmp_data_.d_name);
				}
			}
		}
		closedir(handler_);
		handler_ = 0;
	}
#endif

	inline void filefind::merge_path(const char* dir, const char* sub_dir) {
		size_t len = strlen(dir);
		size_t len2 = strlen(sub_dir);
#if defined(WIN32)||defined(_WIN32)
		memmove(tmp_data_.cFileName+len, sub_dir, len2+1);
		memmove(tmp_data_.cFileName, dir, len);
#else
		memmove(tmp_data_.d_name+len, sub_dir, len2+1);
		memmove(tmp_data_.d_name, dir, len);
#endif
	}
}

#endif

