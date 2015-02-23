#ifndef __WUYA_COUNTOF_H__
#define __WUYA_COUNTOF_H__

namespace wuya{
	/**
	 * 返回静态数组的大小
	 * 仅作用于编译时
	 *
	 * @param T      数组类型
	 * @param n      数组大小
	 *
	 * @return 数组大小
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

