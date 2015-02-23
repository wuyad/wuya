#ifndef __WUYA_TIMER_H__
#define __WUYA_TIMER_H__

#include <time.h>
#if defined(WIN32) || defined(_WIN32)
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif

namespace wuya{
/**
 * 按实际使用时间计时
 *
 * @author wuya
 */
#if defined(WIN32) || defined(_WIN32)
	/**
	 * 按实际时钟进行计时
	 *
	 * @author wuya
	 */
	class timer {
	public:
		/**
		 * 开始一个新实例，可选择是否正即开始计时
		 *
		 * @param start  是否立即开始计时
		 */
		timer(bool autostart=false) {
			if (autostart == true) {
				start();
			}

		}
		/**
		 * 开始计时
		 */
		void start() {
			_ftime(&start_);
		}
		/**
		 * 终止计时
		 *
		 * @return 计时器的时长
		 */
		double end() {
			_timeb end_time;
			_ftime(&end_time);
			double use_sec = static_cast<double>(end_time.time-start_.time);
			double use_usec = static_cast<double>(end_time.millitm-start_.millitm);
			return use_sec+use_usec/static_cast<double>(1000);
		}
	protected:
		_timeb start_;
	};
#else
	class timer {
	public:
		/**
		 * 开始一个新实例，可选择是否正即开始计时
		 *
		 * @param start  是否立即开始计时
		 */
		timer(bool autostart=false) {
			if (autostart == true) {
				start();
			}

		}
		/**
		 * 开始计时
		 */
		void start() {
			gettimeofday(&start_, 0);
		}
		/**
		 * 终止计时
		 *
		 * @return 计时器的时长
		 */
		double end() {
			timeval end_time;
			gettimeofday(&end_time, 0);
			double use_sec = static_cast<double>(end_time.tv_sec-start_.tv_sec);
			double use_usec = static_cast<double>(end_time.tv_usec-start_.tv_usec);
			while (use_sec < 0) {
				use_sec += 24*60*60;
			}
			return use_sec+use_usec/static_cast<double>(1000000);
		}
	protected:
		timeval start_;
	};
#endif
	/**
	 * 按占用CPU时间进行计时
	 *
	 * @author wuya
	 */
	class cpu_timer {
	public:
		/**
		 * 开始一个新实例，可选择是否正即开始计时
		 *
		 * @param start  是否立即开始计时
		 */
		cpu_timer(bool autostart=false) {
			if (autostart == true)
				start_ = clock();
		}
		/**
		 * 开始计时
		 */
		void start() {
			start_ = clock();
		}
		/**
		 * 终止计时
		 *
		 * @return 计时器的时长
		 */
		double end() {
			return double(clock() - start_) / CLOCKS_PER_SEC;
		}
	protected:
		clock_t start_;
	};
}

#endif
