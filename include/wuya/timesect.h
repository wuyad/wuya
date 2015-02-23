#ifndef __WUYA_TIMESECT_H__
#define __WUYA_TIMESECT_H__

#include <wuya/datetime.h>
#include <wuya/timespan.h>
#include <algorithm>

#ifdef max
	#undef max
#endif
#ifdef min
	#undef min
#endif

namespace wuya{
	/**
	 * ʱ����࣬������ĳ��ʱ��㿪ʼ��ĳ��ʱ��������һ��ʱ��
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class timesection {
	public:
		// ���캯��
		timesection();
		// ���캯��,�������Ϊ��ʼʱ��㣬����ʱ���
		timesection(const datetime& time1, const datetime& time2);
		// ���캯��,������һ��ʱ��θ�������
		timesection(const timesection& section);
	public:
		// ����ʱ��ε�ʱ��
		timespan get_span() const;
		// ���������������00:00:00�Ĵ���
		int  get_days() const;
		// �ж��Ƿ񽻲�
		bool isacross(timesection& ts) const;
		// �ж�ĳ��ʱ���Ƿ���ʱ����
		bool inslice(const datetime& time) const;
		// �ж��Ƿ������һʱ��
		bool include(const timesection& ts) const;
		// �ϲ�����ʱ���
		void union_timesection(const timesection& ts);
		// ����
		void intersection(const timesection& ts);
	public:
		// ʱ���Ƿ���Ч
		bool valid() const;
		// ����ʱ�ε���ʼ����ֹʱ�以��
		void swap();
		// ȡʱ����ʱ��
		datetime starttime() const;
		// ȡʱ����ʱ��
		datetime endtime() const;
	public:
		// ���ز�����=����һ��ʱ�θ�����һ��ʱ��
		const timesection& operator=(const timesection& section);
		bool operator==(const timesection& ts) const;
		bool operator!=(const timesection& ts) const;
	protected:
		// ��ʱ�ε���ʼʱ��
		datetime time1_;

		// ��ʱ�εĽ���ʱ��
		datetime time2_;
	};
}

//.............................ʵ�ֲ���.............................//
namespace wuya{
	inline timesection::timesection() {
	}

	inline timesection::timesection(const datetime& time1, const datetime& time2) {
		time1_ = time1;
		time2_ = time2;
	}

	inline timesection::timesection(const timesection& section) {
		time1_ = section.time1_;
		time2_ = section.time2_;
	}

	inline const timesection& timesection::operator=(const timesection& section) {
		time1_ = section.time1_;
		time2_ = section.time2_;
		return *this;
	}

	inline bool timesection::valid() const {
		return(time1_ <= time2_);
	}

	inline void timesection::swap() {
		datetime time;
		time = time1_;
		time1_ = time2_;
		time2_ = time;
	}

	inline datetime timesection::starttime() const {
		return 	time1_;
	}

	inline datetime timesection::endtime() const {
		return time2_;
	}

	inline timespan timesection::get_span() const {
		return(time2_ - time1_);
	}

	inline int timesection::get_days() const {
		if (valid()) {
			datetime t1(time1_.year(), time1_.month(), time1_.day());
			datetime t2(time2_.year(), time2_.month(), time2_.day());
			return(t2-t1).get_days();
		} else {
			return -1;
		}
	}

	inline bool timesection::isacross(timesection& ts) const {
		if (time1_ <= ts.time1_) {
			return( time2_ >= ts.time1_ );
		} else {
			return( time1_ <= ts.time2_ );
		}
	}

	inline bool timesection::inslice(const datetime& time) const {
		return(time1_ <= time && time2_ >= time );
	}

	inline bool timesection::include(const timesection& ts) const {
		if (time1_ <= ts.time1_) {
			return(time2_ >= ts.time2_);
		} else {
			return false;
		}
	}

	inline void timesection::union_timesection(const timesection& ts) {
		time1_ = std::min(time1_, ts.time1_);
		time2_ = std::max(time2_, ts.time2_);
	}

	// ����
	inline void timesection::intersection(const timesection& ts) {
		datetime t1 = std::max(time1_, ts.time1_);
		datetime t2 = std::min(time2_, ts.time2_);
		if (t2>=t1) {
			time1_ = t1;
			time2_ = t2;
		}
	}

	inline bool timesection::operator==(const timesection& ts) const {
		return time1_== ts.time1_ && time2_ == ts.time2_;
	}

	inline bool timesection::operator!=(const timesection& ts) const {
		return !(ts==*this);
	}
}
#endif


