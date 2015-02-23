#ifndef __WUYA_COUNTOF_H__
#define __WUYA_COUNTOF_H__

namespace wuya{
	/**
	 * ���ؾ�̬����Ĵ�С
	 * �������ڱ���ʱ
	 *
	 * @param T      ��������
	 * @param n      �����С
	 *
	 * @return �����С
	 */
	template <class T, int n>
	inline int countof( T(&)[n] ) {
		return n;
	}

	template<class T, int n>
	unsigned char (&countof2_func(T(&)[n]))[n];

	#define static_countof(x) static_cast<int>(sizeof(wuya::countof2_func((x))))
}

#endif

