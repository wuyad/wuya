#ifndef __WUYA_STATIC_ASSERT_H__
#define __WUYA_STATIC_ASSERT_H__

namespace wuya{
	template <bool> struct compile_time_error;
	template <>     struct compile_time_error<true> {
	};
}

/**
 * ¾²Ì¬Ê±¶ÏÑÔ
 */
#define STATIC_ASSERT(expr, msg) \
    { wuya::compile_time_error<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }

#endif

