#ifndef __WUYA_WILDCARD_H__
#define __WUYA_WILDCARD_H__

#include <cstring>

namespace wuya {
	/**
	 * ƥ���ַ���
	 *
	 * @param src     Դ�ַ���
	 * @param matcher ƥ���ӣ��ɰ�����?���롰*��
	 * @param case_sensitive
	 *                �Ƿ����ִ�Сд
	 *
	 * @return �Ƿ�ƥ��
	 */
	bool match(const char* src, const char* matcher, bool case_sensitive=true);
	/**
	 * ����ƥ��
	 * ���԰������ƥ���ӣ���sep�ָ���
	 *
	 * @param src     Դ�ַ���
	 * @param matcher ƥ���ӣ��ɰ�����?���롰*��
	 * @param logic   ��ƥ����֮����߼���ϵ
	 *                0 ��ʾ�����ߡ�����
	 *                1 ��ʾ�����ҡ�����
	 * @param case_sensitive
	 *                �Ƿ��Сд����
	 * @param sep     �ָ���
	 *
	 * @return �Ƿ�ƥ��
	 */
	bool multimatch(const char* src, const char* matcher, int logic=0,
					bool case_sensitive=true, char sep=',');
}

//.............................ʵ�ֲ���.............................//
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

		if( logic == 0 ) {//���
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
		} else {//����
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

