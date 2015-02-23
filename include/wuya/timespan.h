#ifndef __WUYA_TIMESPAN_H__
#define __WUYA_TIMESPAN_H__

#include <ctime>
#include <wuya/datetime.h>

namespace wuya{
	/**
	 * ʱ�����࣬����һ��û����ʼ�ͽ�����ʱ�����Ϣ������10�룬1��2Сʱ40��
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class timespan {
	public:
		// ���캯��
		timespan();

		//  ���캯��������һ��time_t�����Ͳ�����time�����Լ�
		timespan(std::time_t time);

		//  ���캯�������������������һ��ʱ����
		timespan(long day, int hour, int minu=0, int sec=0);

		//  ���캯��,��һ��ʱ���������Լ�
		timespan(const timespan& span);
	public:
		void set(time_t time);
		void set(long day, int hour, int minu, int sec);
	public:
		//  ȡ��ʱ����������
		long get_days() const;
		//  ȡ��ʱ��������Сʱ
		long get_total_hours() const;
		//  ȡ��ʱ���������һ���Сʱ
		int get_hours() const;
		//  ȡ��ʱ�������ܷ���
		long get_total_minutes() const;
		//  ȡ��ʱ�������һ��Сʱ�ķ���
		int get_minutes() const;
		//  ȡ��ʱ������������
		long get_total_seconds() const;
		//  ȡ��ʱ�������һ���ӵ�����
		int get_seconds() const;
	public:
		//  ��ֵ
		const timespan& operator=(const timespan& span);
		//  ����2��ʱ����������ʱ����
		timespan operator-(timespan& timeSpan) const;
		//  �����ʱ����������һ��ʱ�������ʱ����
		timespan operator+(timespan& timeSpan) const;
		//  �����ʱ����������һ��ʱ�������ʱ���,���Ѽ�������������
		const timespan& operator+=(timespan& timeSpan);
		//  ����2��ʱ����������ʱ����,���Ѽ�������������
		const timespan& operator-=(timespan& timeSpan);
		//  �жϴ�ʱ�����Ƿ�����һʱ�������
		bool operator==(timespan timeSpan) const;
		//  �жϴ�ʱ�����Ƿ�����һʱ���������
		bool operator!=(timespan timeSpan) const;
		//  �жϴ�ʱ�����Ƿ�С����һʱ����
		bool operator<(timespan timeSpan) const;
		//  �жϴ�ʱ�����Ƿ������һʱ����
		bool operator>(timespan timeSpan) const;
		//  �жϴ�ʱ�����Ƿ�С�ڻ������һʱ����
		bool operator<=(timespan timeSpan) const;
		//  �жϴ�ʱ�����Ƿ���ڻ������һʱ����
		bool operator>=(timespan timeSpan) const;
	protected:
		//  �����ʱ������Ϣ�ı���
		time_t span_;

		friend datetime operator-(const datetime& time, const timespan& span);
		friend datetime operator+(const datetime& time, const timespan& span);
	};

	datetime operator-(const datetime& time, const timespan& span);
	timespan operator-(const datetime& time1, const datetime& time2);
	datetime operator+(const datetime& time, const timespan& span);
	timespan operator+(const datetime& time1, const datetime& time2);
}

//.............................ʵ�ֲ���.............................//
namespace wuya{
	inline timespan::timespan():span_(0) {
	}

//  ���캯��������һ��time_t�����Ͳ�����time�����Լ�
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



