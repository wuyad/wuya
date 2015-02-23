#ifndef __WUYA_WILDCARD_H__
#define __WUYA_WILDCARD_H__

#include <cstring>

namespace wuya {
	/**
	 * 匹配字符串
	 *
	 * @param src     源字符串
	 * @param matcher 匹配子，可包含“?”与“*”
	 * @param case_sensitive
	 *                是否区分大小写
	 *
	 * @return 是否匹配
	 */
	bool match(const char* src, const char* matcher, bool case_sensitive=true);
	/**
	 * 多重匹配
	 * 可以包含多个匹配子，以sep分隔开
	 *
	 * @param src     源字符串
	 * @param matcher 匹配子，可包含“?”与“*”
	 * @param logic   各匹配子之间的逻辑关系
	 *                0 表示“或者”语意
	 *                1 表示“并且”语意
	 * @param case_sensitive
	 *                是否大小写敏感
	 * @param sep     分隔符
	 *
	 * @return 是否匹配
	 */
	bool multimatch(const char* src, const char* matcher, int logic=0,
					bool case_sensitive=true, char sep=',');
}

//.............................实现部分.............................//
namespace wuya {
	inline bool match(const char* src, const char* matcher, bool case_sensitive) {
		// Written by Jack Handy - jakkhandy@hotmail.com
		const char *cp = NULL, *mp = NULL;

		while( (*src) && (*matcher != '*') ) {
			if( (*matcher != *src) && (*matcher != '?') ) {
				return false;
			}
			matcher++;
			src++;
		}

		while( *src ) {
			if( *matcher == '*' ) {
				if( !*++matcher ) {
					return 1;
				}
				mp = matcher;
				cp = src+1;
			} else if( (*matcher == *src) || (*matcher == '?') ) {
				matcher++;
				src++;
			} else {
				matcher = mp;
				src = cp++;
			}
		}

		while( *matcher == '*' ) {
			matcher++;
		}
		return !*matcher;
	}

	inline bool multimatch(const char* src, const char* matcher, int logic,
						   bool case_sensitive, char sep) {
		if( src == 0 || matcher == 0 )
			return false;

		char* sub_match = new char[strlen(matcher)+1];
		bool is_match;

		if( logic == 0 ) {//求或
			is_match = 0;
			int i = 0;
			int j = 0;
			while( 1 ) {
				if( matcher[i] != 0 && matcher[i] != sep )
					sub_match[j++] = matcher[i];
				else {
					sub_match[j] = 0;
					if( j != 0 ) {
						is_match = match(src, sub_match, case_sensitive);
						if( is_match )
							break;
					}
					j = 0;
				}

				if( matcher[i] == 0 )
					break;
				i++;
			}
		} else {//求与
			is_match = 1;
			int i = 0;
			int j = 0;
			while( 1 ) {
				if( matcher[i] != 0 && matcher[i] != sep )
					sub_match[j++] = matcher[i];
				else {
					sub_match[j] = 0;

					is_match = match(src, sub_match, case_sensitive);
					if( !is_match )
						break;

					j = 0;
				}

				if( matcher[i] == 0 )
					break;
				i++;
			}
		}

		delete [] sub_match;

		return is_match;
	}

}

#endif

