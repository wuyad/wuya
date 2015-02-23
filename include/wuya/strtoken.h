#ifndef __WUYA_STRTOKEN_H__
#define __WUYA_STRTOKEN_H__

#include <string>
namespace wuya{
	/**
	 * 分隔字符串，将形如"a,b,c"分隔为"a","b","c"，采用迭代器的方式
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class strtoken {
	protected:
		struct iter_t;
	public:
		typedef iter_t iterator;
		typedef iter_t const_iterator;
	public:
		strtoken();
		strtoken(const char* str, char sep=',');
		strtoken(const char* str, int len, char sep=',');
	public:
		void set(const char* str, char sep=',');
		void set(const char* str, int len, char sep=',');
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
	protected:
		const char* str_;
		char sep_;
		const char* end_;
		struct iter_t {
			const char* begin_;
			std::string value_;
			bool is_new_value_;
			char sep_;
			const char* end_;
			const char* next_;
			iter_t(const char* begin, const char* end, char sep);
			iter_t operator++(int);
			iter_t operator++();
			bool operator==(const iter_t& iter);
			bool operator!=(const iter_t& iter);
			std::string operator*();
			std::string* operator->();
		};
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline strtoken::strtoken():str_(0), end_(0), sep_(',') {
	}
	inline strtoken::strtoken(const char* str, char sep):str_(str), sep_(sep),
	end_(str_+strlen(str_)) {
	}
	inline strtoken::strtoken(const char* str, int len, char sep):str_(str),
	end_(str+len), sep_(sep) {
	}
	inline void strtoken::set(const char* str, char sep) {
		str_ = str;
		sep_ = sep;
		end_ = str_+strlen(str_);
	}
	inline void strtoken::set(const char* str, int len, char sep) {
		str_ = str;
		sep_ = sep;
		end_ = str_ + len;
	}
	inline strtoken::iterator strtoken::begin() {
		return iter_t(str_-1, end_, sep_);
	}
	inline strtoken::const_iterator strtoken::begin() const {
		return iter_t(str_-1, end_, sep_);
	}
	inline strtoken::iterator strtoken::end() {
		return iter_t(end_, end_, sep_);
	}
	inline strtoken::const_iterator strtoken::end() const {
		return iter_t(end_, end_, sep_);
	}


	inline strtoken::iter_t::iter_t(const char* begin, const char* end, char sep):begin_(begin),
	is_new_value_(true),sep_(sep),end_(end),next_(0) {
	}
	inline strtoken::iter_t strtoken::iter_t::operator++(int) {
		iter_t old = *this;
		operator++();
		return old;
	}
	inline strtoken::iter_t strtoken::iter_t::operator++() {
		if (next_ == 0) {
			while (begin_<end_ && *(++begin_) != sep_);
		} else {
			begin_ = next_;
		}
		is_new_value_ = true;
		return *this;
	}
	inline bool strtoken::iter_t::operator==(const iter_t& iter) {
		return begin_ == iter.begin_;
	}
	inline bool strtoken::iter_t::operator!=(const iter_t& iter) {
		return begin_ != iter.begin_;
	}
	inline std::string strtoken::iter_t::operator*() {
		if (is_new_value_) {
			const char* p = begin_;
			while (p<end_ && *(++p) != sep_);
			value_.assign(begin_+1, (std::string::size_type)(p-begin_)-1);
			next_ = p;
			is_new_value_ = false;
		}
		return value_;
	}
	inline std::string* strtoken::iter_t::operator->() {
		operator*();
		return &value_;
	}
}
#endif


