#ifndef __WUYA_TIMESPAN_H__
#define __WUYA_TIMESPAN_H__

#include <ctime>
#include <wuya/datetime.h>

namespace wuya{
	/**
	 * 时间间隔类，描述一段没有起始和结束的时间段信息，比如10秒，1天2小时40秒
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class timespan {
	public:
		// 构造函数
		timespan();

		//  构造函数，并将一个time_t的类型不变量time赋给自己
		timespan(std::time_t time);

		//  构造函数，根据输入参数构造一个时间间隔
		timespan(long day, int hour, int minu=0, int sec=0);

		//  构造函数,把一个时间间隔赋给自己
		timespan(const timespan& span);
	public:
		void set(time_t time);
		void set(long day, int hour, int minu, int sec);
	public:
		//  取此时间间隔的天数
		long get_days() const;
		//  取此时间间隔的总小时
		long get_total_hours() const;
		//  取此时间间隔的最后一天的小时
		int get_hours() const;
		//  取此时间间隔的总分钟
		long get_total_minutes() const;
		//  取此时间间隔最后一个小时的分钟
		int get_minutes() const;
		//  取此时间间隔的总秒数
		long get_total_seconds() const;
		//  取此时间间隔最后一分钟的秒数
		int get_seconds() const;
	public:
		//  赋值
		const timespan& operator=(const timespan& span);
		//  计算2个时间间隔中相差的时间间隔
		timespan operator-(timespan& timeSpan) const;
		//  计算此时间间隔加上另一个时间间隔后的时间间隔
		timespan operator+(timespan& timeSpan) const;
		//  计算此时间间隔加上另一个时间间隔后的时间间,并把计算结果赋给自身
		const timespan& operator+=(timespan& timeSpan);
		//  计算2个时间间隔中相差的时间间隔,并把计算结果赋给自身
		const timespan& operator-=(timespan& timeSpan);
		//  判断此时间间隔是否于另一时间间隔相等
		bool operator==(timespan timeSpan) const;
		//  判断此时间间隔是否于另一时间间隔不相等
		bool operator!=(timespan timeSpan) const;
		//  判断此时间间隔是否小于另一时间间隔
		bool operator<(timespan timeSpan) const;
		//  判断此时间间隔是否大于另一时间间隔
		bool operator>(timespan timeSpan) const;
		//  判断此时间间隔是否小于或等于另一时间间隔
		bool operator<=(timespan timeSpan) const;
		//  判断此时间间隔是否大于或等于另一时间间隔
		bool operator>=(timespan timeSpan) const;
	protected:
		//  计算此时间间隔信息的变量
		time_t span_;

		friend datetime operator-(const datetime& time, const timespan& span);
		friend datetime operator+(const datetime& time, const timespan& span);
	};

	datetime operator-(const datetime& time, const timespan& span);
	timespan operator-(const datetime& time1, const datetime& time2);
	datetime operator+(const datetime& time, const timespan& span);
	timespan operator+(const datetime& time1, const datetime& time2);
}

//.............................实现部分.............................//
namespace wuya{
	inline timespan::timespan():span_(0) {
	}

//  构造函数，并将一个time_t的类型不变量time赋给自己
	inline timespan::timespan(time_t time) {
		set(time);
	}

	inline timespan::timespan(long day, int hour, int minu, int sec) {
		set(day, hour, minu, sec);
	}

	inline timespan::timespan(const timespan& span) {
		span_ = span.span_;
	}

	inline void timespan::set(time_t time){
		span_ = time;
	}

	inline void timespan::set(long day, int hour, int minu, int sec){
		span_ = sec + 60* (minu + 60* (hour + 24* day));
	}

	inline const timespan& timespan::operator=(const timespan& span) {
		span_ = span.span_; return *this;
	}

	inline long timespan::get_days() const {
		return static_cast<long>(span_) / (24*3600L);
	}

	inline long timespan::get_total_hours() const {
		return static_cast<long>(span_)/3600;
	}

	inline int timespan::get_hours() const {
		return(int)(get_total_hours() - get_days()*24);
	}

	inline long timespan::get_total_minutes() const {
		return static_cast<long>(span_)/60;
	}

	inline int timespan::get_minutes() const {
		return (int)(get_total_minutes() - get_total_hours()*60);
	}

	inline long timespan::get_total_seconds() const {
		return static_cast<long>(span_);
	}

	inline int timespan::get_seconds() const {
		return (int)(get_total_seconds() - get_total_minutes()*60);
	}

	inline timespan timespan::operator-(timespan& timeSpan) const {
		return (static_cast<std::time_t>(span_ - timeSpan.span_));
	}

	inline timespan timespan::operator+(timespan& timeSpan) const {
		return (static_cast<std::time_t>(span_ + timeSpan.span_));
	}

	inline const timespan& timespan::operator+=(timespan& timeSpan) {
		span_ += timeSpan.span_; return *this;
	}

	inline const timespan& timespan::operator-=(timespan& timeSpan) {
		span_ -= timeSpan.span_; return *this;
	}

	inline bool timespan::operator==(timespan timeSpan) const {
		return span_ == timeSpan.span_;
	}

	inline bool timespan::operator!=(timespan timeSpan) const {
		return span_ != timeSpan.span_;
	}

	inline bool timespan::operator<(timespan timeSpan) const {
		return span_ < timeSpan.span_;
	}

	inline bool timespan::operator>(timespan timeSpan) const {
		return span_ > timeSpan.span_;
	}

	inline bool timespan::operator<=(timespan timeSpan) const {
		return span_ <= timeSpan.span_;
	}

	inline bool timespan::operator>=(timespan timeSpan) const {
		return span_ >= timeSpan.span_;
	}

	inline datetime operator-(const datetime& time, const timespan& span) {
		return (time.get_time() - span.span_);
	}

	inline timespan operator-(const datetime& time1, const datetime& time2) {
		return (time1.get_time() - time2.get_time());
	}

	inline datetime operator+(const datetime& time, const timespan& span) {
		return (time.get_time() + span.span_);
	}

	inline timespan operator+(const datetime& time1, const datetime& time2) {
		return (time1.get_time() + time2.get_time());
	}

}

#endif



