#ifndef __WUYA_TIMECYCLE_H__
#define __WUYA_TIMECYCLE_H__

#include <wuya/datetime.h>
#include <wuya/timespan.h>
#include <wuya/countof.h>

namespace wuya {
	/**
	 * ʱ�����ڲ���
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class time_cycle_policy {
	public:
		/**
		 * ȡ��һ��ʱ���
		 *
		 * @param begin_time ��ʼʱ�䣬�����Ӻ�ʱ��ʼ����
		 * @param inc_begin_time
		 *                   �Ƿ������ʼʱ��
		 *
		 * @return ��һ��ʱ���
		 */
		virtual datetime next(const datetime& begin_time, bool inc_begin_time=false) const = 0;
	};

	/**
	 * ʱ������
	 *
	 * @author wuya
	 */
	class time_cycle {
	public:
		time_cycle();
		time_cycle(const time_cycle_policy* policy);
	public:
		/**
		 * ����ʱ�����ڲ���
		 *
		 * @param policy ʱ�����ڲ���
		 */
		void set(const time_cycle_policy* policy);
		/**
		 * ָ��ʱ�俪ʼ����һ��ʱ���
		 *
		 * @param begin_time ��ʼʱ��
		 * @param inc_begin_time
		 *                   �Ƿ������ʼʱ��
		 *
		 * @return ��һ��ʱ���
		 */
		datetime next(const datetime& begin_time, bool inc_begin_time=false);
		datetime next();
		/**
		 * ����һ��ʱ���ļ��ʱ��
		 *
		 * @param begin_time ��ʼʱ��
		 * @param inc_begin_time
		 *                   �Ƿ������ʼʱ��
		 *
		 * @return ���ʱ�䣬�����
		 */
		long next_duration(const datetime& begin_time, bool inc_begin_time=false);
		long next_duration();
	public:
		/**
		 * ȡʱ�����ڲ���
		 *
		 * @return ʱ�����ڲ���
		 */
		const time_cycle_policy* get_policy() const;
	protected:
		const time_cycle_policy* policy_;
		datetime last_datetime_;
	};

	/**
	 * ����windows����ƻ��Ķ���ʵ�ֵ�һ��ʱ�����ڲ���
	 * ��������������յ�ѭ����λ���£��ܣ��գ���һ���ڵ�ѭ����λ��Сʱ�����ӣ��룩
	 *
	 * @author wuya
	 */
	class at_time_cycle_policy : public time_cycle_policy {
	public:
		enum cycle_unit {
			MONTH, MONTH_WEEK, WEEK, DAY, CERTAIN
		};
		enum day_policy {
			CYCLE, NO_CYCLE
		};
		enum week_name {
			SUN=0x1, MON=0x2, TUE=0x4, WED=0x8, THU=0x10, FRI=0x20, SAT=0x40, ALL_DAY=0xFF
		};
		enum day_cycle_unit {
			HOUR, MINUTE, SECOND
		};

		at_time_cycle_policy();
		/**
		 * �������ڲ���
		 * ֧�����¼������ڲ��ԣ�
		 * MONTH--ÿ�µ�n��(n��Ϊ��������ָ"������n��"��
		 * MONTH_WEEK--ÿ�µ�n������m(n��Ϊ��������ָ"������n������m��m����Ϊĳһ�죩
		 * WEEK--ÿn������m��m��Ϊ���죩
		 * DAY--ÿn��
		 * CERTAIN--ָ�����ڣ�certain_day)
		 * ���ϱ����е�n��Ϊ����every��mΪ����days
		 *
		 * @param type   ���ڲ���
		 * @param every  ����������
		 * @param days   ����������
		 */
		at_time_cycle_policy(cycle_unit unit, int every = 1, int days=ALL_DAY);
		at_time_cycle_policy(const datetime& certain_day);
	public:
		void set(cycle_unit unit, int every = 1, int days=ALL_DAY);
		void set(const datetime& certain_day);
		/**
		 *
		 * @param type
		 * @param start_time ��ʼʱ�䣬��ʱ�䲿����Ч
		 * @param end_time   ����ʱ�䣬��ʱ�䲿����Ч
		 * @param unit
		 * @param span
		 */
		void set_day_policy(day_policy type, const datetime& start_time=datetime(2000,1,1),
							const datetime& end_time=datetime::current_time(),
							int span=0, day_cycle_unit day_unit = HOUR);
		void set_day_policy(day_policy type, int span=0, day_cycle_unit day_unit = HOUR);

		datetime next(const datetime& begin_time, bool inc_begin_time=true) const;
	protected:
		datetime start_time_;
		datetime end_time_;
		cycle_unit unit_;
		day_cycle_unit day_unit_;
		day_policy day_policy_;
		int every_;
		int oneday_span_;
		datetime certain_;
		int days_;
	private:
		void init();
		datetime oneday_next(const datetime& time, const datetime& begin_time,
							 const datetime& end_time, bool inc_begin_time=false) const;
		void merge_day_time(const datetime& date0, const datetime& time0, datetime& result) const;
		datetime do_month(const datetime& begin_time, bool inc_begin_time) const;
		datetime do_week(const datetime& begin_time, bool inc_begin_time) const;
		datetime do_month_week(const datetime& begin_time, bool inc_begin_time) const;
		datetime do_day(const datetime& begin_time, bool inc_begin_time) const;
		int next_day_of_week(int day) const;
	};
}

//.............................ʵ�ֲ���.............................//
namespace wuya{
	inline time_cycle::time_cycle():policy_(0), last_datetime_(datetime::current_time()) {
	}

	inline time_cycle::time_cycle(const time_cycle_policy* policy):policy_(policy),
	last_datetime_(datetime::current_time()) {
	}

	inline void time_cycle::set(const time_cycle_policy* policy) {
		policy_ = policy;
	}

	inline const time_cycle_policy* time_cycle::get_policy() const {
		return policy_;
	}

	inline datetime time_cycle::next() {
		if (last_datetime_.valid()) {
			return(last_datetime_=policy_->next(last_datetime_));
		} else {
			return last_datetime_;
		}
	}

	inline datetime time_cycle::next(const datetime& begin_time, bool inc_begin_time) {
		return(last_datetime_ = policy_->next(begin_time, inc_begin_time));
	}

	inline long time_cycle::next_duration() {
		if (last_datetime_.valid()) {
			return next_duration(last_datetime_);
		} else {
			return -1L;
		}
	}

	inline long time_cycle::next_duration(const datetime& begin_time, bool inc_begin_time) {
		datetime next = policy_->next(begin_time, inc_begin_time);
		if (next.valid() && next>=begin_time) {
			last_datetime_ = next;
			return(next-begin_time).get_total_seconds();
		} else {
			last_datetime_.set(-1);
			return -1L;
		}
	}


	inline at_time_cycle_policy::at_time_cycle_policy():every_(1),unit_(MONTH),
	day_policy_(NO_CYCLE),day_unit_(HOUR),oneday_span_(0),days_(0) {
		init();
	}

	inline at_time_cycle_policy::at_time_cycle_policy(cycle_unit unit, int every, int days):
	day_policy_(NO_CYCLE),day_unit_(HOUR),oneday_span_(0),days_(days),every_(every),unit_(unit) {
		init();
	}

	inline at_time_cycle_policy::at_time_cycle_policy(const datetime& certain_day):every_(1),
	unit_(CERTAIN),day_policy_(NO_CYCLE),day_unit_(HOUR),oneday_span_(0),
	days_(0),certain_(certain_day) {
		init();
	}

	inline void at_time_cycle_policy::set(cycle_unit unit, int every, int days) {
		unit_ = unit;
		every_ = every;
		days_ = days;
	}

	inline void at_time_cycle_policy::set(const datetime& certain_day) {
		unit_ = CERTAIN;
		certain_ = certain_day;
	}

	inline void at_time_cycle_policy::set_day_policy(day_policy type, int span,
													 day_cycle_unit day_unit) {
		day_policy_ = type;
		day_unit_ = day_unit;
		oneday_span_ = span;
	}

	inline void at_time_cycle_policy::set_day_policy(day_policy type,
													 const datetime& start_time,
													 const datetime& end_time,
													 int span, day_cycle_unit day_unit) {
		day_policy_ = type;
		start_time_ = start_time;
		end_time_ = end_time;
		day_unit_ = day_unit;
		oneday_span_ = span;
	}

	inline datetime at_time_cycle_policy::next(const datetime& begin_time,
											   bool inc_begin_time) const {
		switch (unit_) {
		case MONTH:
			return do_month(begin_time, inc_begin_time);
			break;
		case MONTH_WEEK:
			return do_month_week(begin_time, inc_begin_time);
			break;
		case WEEK:
			return do_week(begin_time, inc_begin_time);
			break;
		case DAY:
			return do_day(begin_time, inc_begin_time);
			break;
		case CERTAIN:
			if (day_policy_ == NO_CYCLE) {
				if (inc_begin_time?begin_time<=certain_:begin_time<certain_) {
					return certain_;
				}
			} else {
				datetime end_time;
				merge_day_time(certain_, end_time_, end_time);
				return oneday_next(begin_time, certain_, end_time, inc_begin_time);
			}
			break;
		}
		return datetime(-1);
	}

	inline datetime at_time_cycle_policy::oneday_next(const datetime& time,
													  const datetime& begin_time,
													  const datetime& end_time,
													  bool inc_begin_time) const {
		if (inc_begin_time?time<=begin_time:time<begin_time) {
			return begin_time;
		}
		if (time>end_time) {
			return datetime(-1);
		}
		long hour=0,minu=0,sec=0;
		switch (day_unit_) {
		case HOUR:
			hour = oneday_span_;
			break;
		case MINUTE:
			minu = oneday_span_;
			break;
		case SECOND:
			sec = oneday_span_;
			break;
		}
		datetime next_time = begin_time + timespan(0,hour,minu,sec);
		if (next_time>end_time) {
			return datetime(-1);
		}
		if (next_time > time) {
			return next_time;
		}
		return oneday_next(time, next_time, end_time, inc_begin_time);
	}

	inline datetime at_time_cycle_policy::do_month(const datetime& begin_time,
												   bool inc_begin_time) const {
		int real_day = every_;
		int year = begin_time.year();
		int month = begin_time.month();
		if (every_ < 0) {
			int d = 1;
			int y = year;
			int m = month;
			if (m==12) {
				++y;
				m = 1;
			} else {
				++m;
			}
			datetime tmp(y, m, d);
			tmp = tmp - timespan(-(every_+1), 0,0,1);
			real_day = tmp.day();
		}
		datetime next;
		if (real_day == begin_time.day()) {
			next.set(year, month, real_day, begin_time.hour(), begin_time.minute(),
					 begin_time.second());
		} else {
			next.set(year, month, real_day);
		}
		if (inc_begin_time?begin_time<=next:begin_time<next) {
			return next;
		}
		if (day_policy_ == CYCLE) {
			datetime start;
			datetime end;
			merge_day_time(next, start_time_, start);
			if (begin_time>start_time_) {
				start = begin_time;
			}
			merge_day_time(next, end_time_, end);
			datetime d_next = oneday_next(next, start, end, inc_begin_time);
			if (d_next.valid()) {
				return d_next;
			}
		}
		if (month==12) {
			++year;
			month = 1;
		} else {
			++month;
		}
		if (every_ < 0) {
			int d = 1;
			int y = year;
			int m = month;
			if (m==12) {
				++y;
			} else {
				++m;
			}
			datetime tmp(y, m, d);
			tmp = tmp - timespan(-(every_+1), 0,0,1);
			real_day = tmp.day();
		}
		next.set(year, month, real_day);
		return next;
	}

	inline datetime at_time_cycle_policy::do_week(const datetime& begin_time,
												  bool inc_begin_time) const {
		if (days_ == 0) {
			return datetime(-1);
		}
		int week_day = begin_time.get_day_of_week();
		int next_day;
		datetime next_time;
		if ( (next_day=next_day_of_week(week_day)) != -1) {
			if (next_day == week_day) {
				if (day_policy_ == NO_CYCLE && inc_begin_time) {
					return begin_time;
				}
				if (day_policy_ == CYCLE) {
					datetime next;
					datetime start;
					datetime end;
					merge_day_time(begin_time, start_time_, start);
					merge_day_time(begin_time, end_time_, end);
					if ((next=oneday_next(begin_time, start, end, inc_begin_time)).valid()) {
						return next;
					}
				}
				if (week_day < 7) {
					next_time = begin_time+timespan(1,0);
					return do_week(next_time, inc_begin_time);
				}
			} else {
				if (day_policy_ == NO_CYCLE) {
					next_time = begin_time + timespan(next_day-week_day, 0);
					return next_time;
				} else {
					next_time.set(begin_time.year(), begin_time.month(), begin_time.day());
					next_time = next_time + timespan(next_day-week_day, 0);
					merge_day_time(next_time, start_time_, next_time);
					return next_time;
				}
			}
		}
		return do_week(begin_time+timespan(7*every_-(week_day-1), 0), inc_begin_time);
	}

	inline datetime at_time_cycle_policy::do_month_week(const datetime& begin_time,
														bool inc_begin_time) const {
		if (days_ == 0) {
			return datetime(-1);
		}
		int year = begin_time.year();
		int month = begin_time.month();
		int day = begin_time.day();
		int sw_day = 1;
		for (int i=0; i<7; ++i) {
			if (days_>>i == 1) {
				sw_day = i+1;
				break;
			}
		}
		datetime first_day;
		if (every_>0) {
			first_day.set(year, month, 1, begin_time.hour(), begin_time.minute(),
						  begin_time.second());
			int w_day = first_day.get_day_of_week();
			int defer = sw_day-w_day;
			if (defer < 0) {
				defer += 7;
			}
			first_day = first_day + timespan(defer+(every_-1)*7, 0);
			if (first_day.month() > month) {
				return datetime(-1);
			}
		} else {
			int y = year;
			int m = month;
			if (month==12) {
				++y;
			} else {
				++m;
			}
			first_day.set(y, m, 1, begin_time.hour(), begin_time.minute(),
						  begin_time.second());
			first_day = first_day - timespan(0,begin_time.hour(), begin_time.minute(),
											 begin_time.second()+1);
			first_day.set(first_day.year(), first_day.month(), first_day.day(),
						  begin_time.hour(), begin_time.minute(),begin_time.second());
			int w_day = first_day.get_day_of_week();
			int defer = sw_day-w_day;
			if (defer > 0) {
				defer -= 7;
			}
			first_day = first_day + timespan(defer+(every_+1)*7, 0);
			if (first_day.month() < month) {
				return datetime(-1);
			}
		}
		if (inc_begin_time?begin_time <= first_day:begin_time<first_day) {
			if (day_policy_ == NO_CYCLE) {
				return first_day;
			} else {
				return datetime(first_day.year(), first_day.month(), first_day.day());
			}
		}
		if (day_policy_ == CYCLE && begin_time.day()==first_day.day()) {
			datetime next;
			datetime start;
			datetime end;
			merge_day_time(begin_time, start_time_, start);
			merge_day_time(begin_time, end_time_, end);
			if ((next=oneday_next(begin_time, start, end, inc_begin_time)).valid()) {
				return next;
			}
		}
		if (month==12) {
			++year;
		} else {
			++month;
		}
		return do_month_week(datetime(year, month, 1, begin_time.hour(), begin_time.minute(),
									  begin_time.second()), inc_begin_time);
	}

	inline datetime at_time_cycle_policy::do_day(const datetime& begin_time,
												 bool inc_begin_time) const {
		datetime next = begin_time;
		if (day_policy_ == NO_CYCLE) {
			return inc_begin_time?begin_time:next + timespan(every_, 0);;
		} else {
			datetime start;
			datetime end;
			merge_day_time(next, start_time_, start);
			merge_day_time(next, end_time_, end);
			if ((next=oneday_next(begin_time, start, end, inc_begin_time)).valid()) {
				return next;
			}
			merge_day_time(begin_time, start_time_, next);
			return(next + timespan(every_, 0));
		}
	}

	inline void at_time_cycle_policy::merge_day_time(const datetime& date0, const datetime& time0,
													 datetime& result) const {
		result.set(date0.year(), date0.month(), date0.day(), time0.hour(), time0.minute(),
				   time0.second());
	}

	inline void at_time_cycle_policy::init() {
		datetime now = datetime::current_time();
		start_time_.set(now.year(), now.month(), now.day());
		end_time_.set(now.year(), now.month(), now.day(), 23, 59, 59);
		days_ &= 0x7f;
		// ֻ��һ����Ч
		if (unit_==MONTH_WEEK) {
			// ֻ�������λ
			days_ = (~days_)+1&days_;
		}
	}

	inline int at_time_cycle_policy::next_day_of_week(int day) const {
		for (int i=day; i<8;++i) {
			if ((days_ & (1<<(i-1))) != 0) {
				return i;
			}
		}
		return -1;
	}
}

#endif

