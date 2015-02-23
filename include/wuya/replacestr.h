#ifndef __WUYA_REPLACESTR_H__
#define __WUYA_REPLACESTR_H__

#include <string>
#include <string.h>
#include <algorithm>
#ifdef max
	#undef max
#endif
namespace wuya{
	/**
	 * È«¾Ö×Ö·û´®Ìæ»»
	 *
	 * @param src    Ô´×Ö·û´®£¬µ÷ºó±»ÐÞ¸Ä
	 * @param from   ÐèÌæ»»×Ö·û´®
	 * @param to     Ìæ»»Îª
	 *
	 * @return Ìæ»»¸öÊý
	 */
	int replace_all(std::string& src, const char* from, const char* to);
	/**
	 * È«¾Ö×Ö·û´®Ìæ»»
	 *
	 * @param src    Ô´×Ö·û´®
	 * @param from   ÐèÌæ»»×Ö·û´®
	 * @param to     Ìæ»»Îª
	 * @param dest   Ìæ»»ºóµÄ×Ö·û´®
	 *
	 * @return Ìæ»»¸öÊý
	 */
	int replace_all(const char* src, const char* from, const char* to, std::string& dest);
}

//.............................ÊµÏÖ²¿·Ö.............................//
namespace wuya{
	inline int replace_all(std::string& src, const char* from, const char* to) {
		return replace_all(src.c_str(), from, to, src);
	}

	inline int replace_all(const char* src, const char* from, const char* to, std::string& dest) {
		int ret = 0;
		std::string tmp;
		size_t len_to = strlen(to);
		const size_t& len_buf = std::max(len_to, (size_t)256);
		char* buf = new char[len_buf];

		const char* pend = src+strlen(src);
		const char* qend = from+strlen(from);
		char* rend = buf+len_buf;
		const char* p = src;
		const char* q = from;
		char* r = buf;
		const char* begin_p_tmp = p;
		do {
			if (*p == *q && q<qend) {
				++q;
			} else {
				if (begin_p_tmp<p) {
					if (q==qend) {
						tmp.append(buf, r);
						memcpy(buf, to, len_to);
						r = buf+len_to;
						*r = *p;
						++r;
						++ret;
						begin_p_tmp = p+1;
					} else {
						tmp.append(buf, r);
						r = buf;
						tmp.append(1, *begin_p_tmp);
						p = begin_p_tmp;
						begin_p_tmp = p+1;
					}
					q = from;
				} else {
					*r = *p;
					++begin_p_tmp;
					++r;
				}
			}
			if (r>=rend) {
				tmp.append(buf, rend);
				r = buf;
			}
		}while (++p<pend);
		tmp.append(buf, r);
		if (begin_p_tmp<p) {
			if (q == qend) {
				tmp.append(to);
				++ret;
			} else {
				tmp.append(begin_p_tmp, p);
			}
		}
		delete [] buf;
		dest = tmp;
		return ret;
	}
}
#endif


