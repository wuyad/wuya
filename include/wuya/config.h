#ifndef __WUYA_CONFIG_H__
#define __WUYA_CONFIG_H__

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>

namespace wuya{
	/**
	 * 读写ini文件
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class config {
#define DEFAULT_SECTION_VALUE "^0^system^0^"
	public:
		config();
		// 构造函数，区分大小写
		config( const char* filename, bool can_modify = false);
		~config();
	public:
		template <class T>
		T get(const char* key, const char* section_name=DEFAULT_SECTION_VALUE, T default_value=T()) const;
		template <class T>
		void set(const char* key, const T& value, const char* section_name=DEFAULT_SECTION_VALUE);

		void erase(const char* section_name);
		void erase(const char* key, const char* section_name);
		void open(const char* filename, bool can_modify = false);
	public:
		bool good();
		bool bad();

		void reset();
		void clear();
		int size();
		int key_size(const char* section_name);
        std::vector<const char*> section_names();
        std::vector<const char*> key_names(const char* section_name);

	protected:
		typedef std::pair<std::string, std::string> value_and_comment_pair;
		typedef std::pair< std::string, value_and_comment_pair > key_pair;
		typedef std::map< std::string, value_and_comment_pair > KEY;

		typedef std::pair<KEY, std::string> keys_with_comment_pair;
		typedef std::map< std::string, keys_with_comment_pair > SECTION;
		bool state_;
		bool modified_;
		bool can_modify_;
		const char* filename_;

		SECTION content_;

		friend std::ostream& operator << (std::ostream& os, const config& i);
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline config::config():modified_(false),state_(false),can_modify_(false),filename_(0) {
	}

	inline config::config( const char* filename, bool can_modify):
	filename_(filename),can_modify_(can_modify) {
		open(filename, can_modify);
	}

	inline void config::open( const char* filename, bool can_modify) {
		filename_ = filename;
		can_modify_ = can_modify;
		state_ = true;
		modified_ = false;
		std::ifstream f(filename);
		if (f.fail()) {
			state_ = false;
			return;
		}
		std::string   line;
		std::string   section = DEFAULT_SECTION_VALUE, key, value, comment;
		std::string::size_type pLeft, pRight, tmp_pointer;

		while ( getline( f, line)) {
			if ( line.length()) {
				if (( pLeft = line.find_first_of(";#[=")) != std::string::npos) {
					switch ( line[pLeft]) {
					case '[':
						if ((pRight = line.find_last_of("]")) != std::string::npos &&
							pRight > pLeft) {
							section = line.substr( pLeft + 1, pRight - pLeft - 1);
							if (section == "") {
								state_ = false;
								return;
							}
							content_[section] = std::pair<KEY, std::string>(KEY(), comment);
							comment = "";
						}
						break;
					case '=':
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
						if (key == "") {
							state_ = false;
							return;
						}
						++pLeft;
						pRight = line.length();
						tmp_pointer = pRight-1;
						while(pLeft<pRight && isspace(line.at(pLeft))){
							++pLeft;
						}
						while(tmp_pointer>pLeft && isspace(line.at(tmp_pointer))) {
							--tmp_pointer;
						}
						value = line.substr(pLeft, tmp_pointer-pLeft+1);
						content_[section].first.insert(
													  key_pair(
															  key,
															  value_and_comment_pair(
																					value,
																					comment
																					)
															  )
													  );
						comment = "";
						break;
					case ';':
					case '#':
						comment += (line+'\n');
						break;
					}
				}
			}
		}

		f.close();
		return;
	}

	inline config::~config() {
		using std::cout;
		if (state_ && can_modify_ && modified_) {
			// 写入文件
			std::ofstream f(filename_);
			if (f.fail()) {
				return;
			}
			f << (*this);
			f.close();
		}
	}

	inline bool config::good() {
		return state_;
	}

	inline bool config::bad() {
		return !state_;
	}

	inline void config::reset() {
		state_ = true;
	}

	inline void config::clear() {
		if (state_) {
			content_.clear();
		}
	}

	inline int config::size() {
		if (state_) {
			return static_cast<int>(content_.size());
		} else {
			return -1;
		}
	}

	inline int config::key_size(const char* section_name) {
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

    inline std::vector<const char*> config::section_names(){
		if (state_) {
            std::vector<const char*> v;
			SECTION::const_iterator iter;
            for(iter = content_.begin(); iter!=content_.end(); ++iter ){
                v.push_back((*iter).first.c_str());
            }
            return v;
		}
		return std::vector<const char*>();
    }

    inline std::vector<const char*> config::key_names(const char* section_name){
		if (state_) {
			SECTION::const_iterator iter;
			if ( (iter=content_.find(section_name)) != content_.end()) {
                KEY::const_iterator iter2=(*iter).second.first.begin();
				std::vector<const char*> v;
                for(;iter2!=(*iter).second.first.end();++iter2) {
                    v.push_back((*iter2).first.c_str());
                }
                return v;
			}
		} 
		return std::vector<const char*>();
    }

	inline void config::erase(const char* section_name) {
		if (state_) {
			if (content_.erase(section_name)) {
				modified_ = true;
			}
		}
	}

	inline void config::erase(const char* key, const char* section_name) {
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
	inline void config::set(const char* key, const T& value, const char* section_name) {
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
					std::stringstream trans;
					trans << value;
					std::string tmp_s;
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
	inline T config::get(const char* key, const char* section_name, T default_value) const{
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
					std::stringstream trans((*iter2).second.first);
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

	template <>
	inline const char* config::get<const char*>(const char* key, const char* section_name, const char* default_value) const{
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
					return ((*iter2).second.first).c_str();
				}
			}
		} else {
			return default_value;
		}
	}

	inline std::ostream& operator << (std::ostream& os, const config& i) {
		using std::endl;
		if (i.state_) {
			config::SECTION::const_iterator iter = i.content_.find(DEFAULT_SECTION_VALUE);
			if (iter != i.content_.end()) {
				const config::KEY& k = (*iter).second.first;
				config::KEY::const_iterator iter2 = k.begin();
				while (iter2 != k.end()) {
					os << (*iter2).second.second;
					os << (*iter2).first << '=' << (*iter2).second.first << endl;
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
				os << '[' << (*iter).first << ']' << endl;
				const config::KEY& k = (*iter).second.first;
				config::KEY::const_iterator iter2 = k.begin();
				while (iter2 != k.end()) {
					os << (*iter2).second.second;
					os << (*iter2).first << '=' << (*iter2).second.first << endl;
					++iter2;
				}
				++iter;
			}
		}
		return os;
	}
}


#endif

