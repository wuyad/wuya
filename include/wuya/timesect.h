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
	 * 时间段类，描述从某个时间点开始到某个时间点结束的一段时间
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class timesection {
	public:
		// 构造函数
		timesection();
		// 构造函数,输入参数为起始时间点，结束时间点
		timesection(const datetime& time1, const datetime& time2);
		// 构造函数,并把另一个时间段赋给自身
		timesection(const timesection& section);
	public:
		// 计算时间段的时长
		timespan get_span() const;
		// 横跨天数，即经过00:00:00的次数
		int  get_days() const;
		// 判断是否交叉
		bool isacross(timesection& ts) const;
		// 判断某个时间是否在时段内
		bool inslice(const datetime& time) const;
		// 判断是否包含另一时段
		bool include(const timesection& ts) const;
		// 合并两个时间段
		void union_timesection(const timesection& ts);
		// 交集
		void intersection(const timesection& ts);
	public:
		// 时段是否有效
		bool valid() const;
		// 交换时段的起始，终止时间互换
		void swap();
		// 取时段起时间
		datetime starttime() const;
		// 取时段终时间
		datetime endtime() const;
	public:
		// 重载操作符=，将一个时段赋给另一个时段
		const timesection& operator=(const timesection& section);
		bool operator==(const timesection& ts) const;
		bool operator!=(const timesection& ts) const;
	protected:
		// 此时段的起始时间
		datetime time1_;

		// 此时段的结束时间
		datetime time2_;
	};
}

//.............................实现部分.............................//
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

	// 交集
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


