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
 * ��ʵ��ʹ��ʱ���ʱ
 *
 * @author wuya
 */
#if defined(WIN32) || defined(_WIN32)
	/**
	 * ��ʵ��ʱ�ӽ��м�ʱ
	 *
	 * @author wuya
	 */
	class timer {
	public:
		/**
		 * ��ʼһ����ʵ������ѡ���Ƿ�������ʼ��ʱ
		 *
		 * @param start  �Ƿ�������ʼ��ʱ
		 */
		timer(bool autostart=false) {
			if (autostart == true) {
				start();
			}

		}
		/**
		 * ��ʼ��ʱ
		 */
		void start() {
			_ftime(&start_);
		}
		/**
		 * ��ֹ��ʱ
		 *
		 * @return ��ʱ����ʱ��
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
		 * ��ʼһ����ʵ������ѡ���Ƿ�������ʼ��ʱ
		 *
		 * @param start  �Ƿ�������ʼ��ʱ
		 */
		timer(bool autostart=false) {
			if (autostart == true) {
				start();
			}

		}
		/**
		 * ��ʼ��ʱ
		 */
		void start() {
			gettimeofday(&start_, 0);
		}
		/**
		 * ��ֹ��ʱ
		 *
		 * @return ��ʱ����ʱ��
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
	 * ��ռ��CPUʱ����м�ʱ
	 *
	 * @author wuya
	 */
	class cpu_timer {
	public:
		/**
		 * ��ʼһ����ʵ������ѡ���Ƿ�������ʼ��ʱ
		 *
		 * @param start  �Ƿ�������ʼ��ʱ
		 */
		cpu_timer(bool autostart=false) {
			if (autostart == true)
				start_ = clock();
		}
		/**
		 * ��ʼ��ʱ
		 */
		void start() {
			start_ = clock();
		}
		/**
		 * ��ֹ��ʱ
		 *
		 * @return ��ʱ����ʱ��
		 */
		double end() {
			return double(clock() - start_) / CLOCKS_PER_SEC;
		}
	protected:
		clock_t start_;
	};
}

#endif
