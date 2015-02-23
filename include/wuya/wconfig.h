#ifndef __WUYA_Wwconfig_H__
#define __WUYA_Wwconfig_H__

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>

namespace wuya{
	/**
	 * 读写ini文件，UNICODE版
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class wconfig {
#define DEFAULT_SECTION_VALUE L"^0^system^0^"
	public:
		wconfig();
		// 构造函数，区分大小写
		wconfig( const wchar_t* filename, bool can_modify = false);
		~wconfig();
	public:
		template <class T>
		T get(const wchar_t* key, const wchar_t* section_name=DEFAULT_SECTION_VALUE, const T& default_value=T());
		template <class T>
		void set(const wchar_t* key, const T& value, const wchar_t* section_name=DEFAULT_SECTION_VALUE);

		void erase(const wchar_t* section_name);
		void erase(const wchar_t* key, const wchar_t* section_name);
		void open(const wchar_t* filename, bool can_modify = false);
	public:
		bool good();
		bool bad();

		void reset();
		void clear();
		int size();
		int key_size(const wchar_t* section_name);

	protected:
		typedef std::pair<std::wstring, std::wstring> value_and_comment_pair;
		typedef std::pair< std::wstring, value_and_comment_pair > key_pair;
		typedef std::map< std::wstring, value_and_comment_pair > KEY;

		typedef std::pair<KEY, std::wstring> keys_with_comment_pair;
		typedef std::map< std::wstring, keys_with_comment_pair > SECTION;
		bool state_;
		bool modified_;
		bool can_modify_;
		const wchar_t* filename_;

		SECTION content_;

		friend std::wostream& operator << (std::wostream& os, const wconfig& i);
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline wconfig::wconfig():modified_(false),state_(false),can_modify_(false),filename_(0) {
	}

	inline wconfig::wconfig( const wchar_t* filename, bool can_modify):
	filename_(filename),can_modify_(can_modify) {
		open(filename, can_modify);
	}

	inline void wconfig::open( const wchar_t* filename, bool can_modify) {
		filename_ = filename;
		can_modify_ = can_modify;
		state_ = true;
		modified_ = false;
		std::wifstream f(filename);
		if (f.fail()) {
			state_ = false;
			return;
		}
		std::wstring   line;
		std::wstring   section = DEFAULT_SECTION_VALUE, key, value, comment;
		std::wstring::size_type pLeft, pRight, tmp_pointer;

		while ( getline( f, line)) {
			if ( line.length()) {
				if (( pLeft = line.find_first_of(L";#[=")) != std::wstring::npos) {
					switch ( line[pLeft]) {
					case L'[':
						if ((pRight = line.find_last_of(L"]")) != std::wstring::npos &&
							pRight > pLeft) {
							section = line.substr( pLeft + 1, pRight - pLeft - 1);
							if (section == L"") {
								state_ = false;
								return;
							}
							content_[section] = std::pair<KEY, std::wstring>(KEY(), comment);
							comment = L"";
						}
						break;
					case L'=':
						if (pLeft == 0){
							state_ = false;
							return;
						}
						tmp_pointer = pLeft-1;
						while(tmp_pointer>=0 && isspace(line.at(tmp_pointer))){
							if (tmp_pointer==0){
								state_ = false;
								return;
							}
							--tmp_pointer;
						}
						key = line.substr( 0, tmp_pointer+1);
						if (key == L"") {
							state_ = false;
							return;
						}
						tmp_pointer = pLeft+1;
						pRight = line.length();
						while(tmp_pointer<pRight && isspace(line.at(tmp_pointer))){
							++tmp_pointer;
						}
						value = line.substr( tmp_pointer);
						content_[section].first.insert(
													  key_pair(
															  key,
															  value_and_comment_pair(
																					value,
																					comment
																					)
															  )
													  );
						comment = L"";
						break;
					case L';':
					case L'#':
						comment += (line+L'\n');
						break;
					}
				}
			}
		}

		f.close();
		return;
	}

	inline wconfig::~wconfig() {
		using std::cout;
		if (state_ && can_modify_ && modified_) {
			// 写入文件
			std::wofstream f(filename_);
			if (f.fail()) {
				return;
			}
			f << (*this);
			f.close();
		}
	}

	inline bool wconfig::good() {
		return state_;
	}

	inline bool wconfig::bad() {
		return !state_;
	}

	inline void wconfig::reset() {
		state_ = true;
	}

	inline void wconfig::clear() {
		if (state_) {
			content_.clear();
		}
	}

	inline int wconfig::size() {
		if (state_) {
			return static_cast<int>(content_.size());
		} else {
			return -1;
		}
	}

	inline int wconfig::key_size(const wchar_t* section_name) {
		if (state_) {
			SECTION::const_iterator iter;
			if ( (iter=content_.find(section_name)) == content_.end()) {
				return -1;
			} else {
				return static_cast<int>((*iter).second.first.size());
			}
		} else {
			return -1;
		}
	}

	inline void wconfig::erase(const wchar_t* section_name) {
		if (state_) {
			if (content_.erase(section_name)) {
				modified_ = true;
			}
		}
	}

	inline void wconfig::erase(const wchar_t* key, const wchar_t* section_name) {
		if (state_) {
			SECTION::iterator iter;
			if ( (iter=content_.find(section_name)) == content_.end()) {
				return;
			} else {
				KEY& k = (*iter).second.first;
				if (k.erase(key)) {
					modified_ = true;
				}
			}
		}
	}

	template <class T>
	inline void wconfig::set(const wchar_t* key, const T& value, const wchar_t* section_name) {
		if (state_) {
			SECTION::iterator iter;
			if ( (iter=content_.find(section_name)) == content_.end()) {
				return;
			} else {
				KEY& k = (*iter).second.first;
				KEY::iterator iter2;
				if ( (iter2=k.find(key)) == k.end()) {
					return;
				} else {
					std::wstringstream trans;
					trans << value;
					std::wstring tmp_s;
					if (trans >> tmp_s) {
						(*iter2).second.first = tmp_s;
						modified_ = true;
					} else {
						return;
					}
				}
			}
		}
	}

	template <class T>
	inline T wconfig::get(const wchar_t* key, const wchar_t* section_name, const T& default_value) {
		if (state_) {
			SECTION::const_iterator iter;
			if ( (iter=content_.find(section_name)) == content_.end()) {
				return default_value;
			} else {
				const KEY& k = (*iter).second.first;
				KEY::const_iterator iter2;
				if ( (iter2=k.find(key)) == k.end()) {
					return default_value;
				} else {
					std::wstringstream trans((*iter2).second.first);
					T tmp_t;
					if (trans >> tmp_t) {
						return tmp_t;
					} else {
						return default_value;
					}
				}
			}
		} else {
			return default_value;
		}
	}

	inline std::wostream& operator << (std::wostream& os, const wconfig& i) {
		using std::endl;
		if (i.state_) {
			wconfig::SECTION::const_iterator iter = i.content_.find(DEFAULT_SECTION_VALUE);
			if (iter != i.content_.end()) {
				const wconfig::KEY& k = (*iter).second.first;
				wconfig::KEY::const_iterator iter2 = k.begin();
				while (iter2 != k.end()) {
					os << (*iter2).second.second;
					os << (*iter2).first << L'=' << (*iter2).second.first << endl;
					++iter2;
				}
			}
			iter = i.content_.begin();
			while (iter != i.content_.end()) {
				if (DEFAULT_SECTION_VALUE == (*iter).first) {
					++iter;
					continue;
				}
				os << (*iter).second.second;
				os << L'[' << (*iter).first << L']' << endl;
				const wconfig::KEY& k = (*iter).second.first;
				wconfig::KEY::const_iterator iter2 = k.begin();
				while (iter2 != k.end()) {
					os << (*iter2).second.second;
					os << (*iter2).first << L'=' << (*iter2).second.first << endl;
					++iter2;
				}
				++iter;
			}
		}
		return os;
	}
}


#endif

