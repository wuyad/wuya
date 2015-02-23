#ifndef __WUYA_DATETIME_H__
#define __WUYA_DATETIME_H__

#include <ctime>
#include <ostream>
#include <string>

namespace wuya {
    /**
     * 时间类，描述一个时间点
     *
     * @author wuya
     * @version 1.0.0
     */
    class datetime {
    public:
        datetime();
        // 构造函数，并将一个std::time_t类型的变量赋值给自身
        datetime(std::time_t t);
        // 构造函数，输入值为年，月(1-12)，日(1-31)，时(0-23)，分(0-59)，秒(0-59)，
        // 根据输入参数，生成时间
        datetime(int year, int month, int day, int hour=0, int minu=0, int sec=0, int dst=0);
        // 构造函数，输入为"YYYYMMDDHHMMSS"或"YYYYMMDD"的字符串，根据字符串生成时间
        datetime(const char* date_time);
        // 构造函数,并将一个time类型的变量赋值给自身
        datetime(const datetime& t);
    public:
        // 判断是否闰年
        static bool is_leap_year( int year );
        void set(std::time_t t);
        void set(int year, int month, int day=1, int hour=0, int minu=0, int sec=0, int dst=0);
        void set(const char* date_time);
        // formatting using "C" strftime
        std::string format(const char* fmt) const;
        // 输出日期,格式为"YYYYMMDD"
        std::string date_str() const;
        // 输出时间,格式为"HHMMSS"
        std::string time_str() const;
        // 输出完整的日期时间，格式为"yyyy-mm-dd hh::mm:ss"
        std::string date_time_str() const;
        // 输出完整的日期时间，格式为"YYYYMMDDHHMMSS"
        std::string date_time_str2() const;
    public:
        // 获取当前系统时间
        static datetime current_time();
        // 得到时间(不包括日期)
        std::time_t get_time() const;
        // 是否有效
        bool valid() const;
        // 得到年份
        int year() const;
        // 得到月份，1-12
        int month() const;
        // 得到日期，1-31
        int day() const;
        // 得到小时，0-23
        int hour() const;
        // 得到分钟，0-59
        int minute() const;
        // 得到秒钟，0-59
        int second() const;
        // 得到星期 1=Sun, 2=Mon, ..., 7=Sat
        int get_day_of_week() const;
    public:
        // 将一个std::time_t类型的变量赋值给自身
        datetime& operator=(std::time_t t);
        datetime& operator=(const datetime& t);
        // 判断2个时间是否相等，相等true,不等false
        bool operator==(const datetime& t) const;
        // 判断2个时间是否不相等，不相等true,相等false
        bool operator!=(const datetime& t) const;
        // 判断另一个时间是否比此时间早
        bool operator<(const datetime& t) const;
        // 判断另一个时间是否比此时间晚
        bool operator>(const datetime& t) const;
        // 判断另一个时间是否比此时间早或等于此时间
        bool operator<=(const datetime& t) const;
        // 判断另一个时间是否比此时间晚或等于此时间
        bool operator>=(const datetime& t) const;
    protected:
        // 成员变量，存放此时间变量的时间信息
        std::time_t time_;
    };
}

//.............................实现部分.............................//
namespace wuya {
    // 获取当前系统时间
    inline datetime datetime::current_time() {
        std::time_t temp;
        return datetime(std::time(&temp));
    }

    // 判断是否闰年
    inline bool datetime::is_leap_year( int year ) {
        return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
    }

    inline datetime::datetime():time_(0) {
    }

    // 构造函数，并将一个std::time_t类型的变量赋值给自身
    inline datetime::datetime(std::time_t t) {
        set(t);
    }

    // 构造函数，输入值为年，月，日，时，分，秒，根据输入参数，生成时间
    inline datetime::datetime(int year, int month, int day, int hour, int minu, int sec, int dst) {
        set(year, month, day, hour, minu, sec, dst);
    }

    // 构造函数，输入为"YYYYMMDDHHMMSS"或"YYYYMMDD"的字符串，根据字符串生成时间
    inline datetime::datetime(const char* date_time) {
        set(date_time);
    }

    inline void datetime::set(std::time_t t) {
        time_ = t;
    }

    inline void datetime::set(int year, int month, int day, int hour, int minu, int sec, int dst) {
        struct tm atm;
        atm.tm_year = year - 1900;     // tm_year is 1900 based
        atm.tm_mon = month-1;          // tm_mon is 0 based
        atm.tm_mday = day;
        atm.tm_hour = hour;
        atm.tm_min = minu;
        atm.tm_sec = sec;
        atm.tm_isdst = dst;
        time_ = std::mktime(&atm);
    }

    inline void datetime::set(const char* date_time) {
        struct tm atm;
        memset(reinterpret_cast<char*>(&atm), 0, sizeof(tm));
        atm.tm_mday = 1;

        char buf[5];
        int i = 0;
        while( *date_time != 0 ) {
            memset(buf, 0, 5);
            switch( i ) {
            case 0:
                memcpy(buf, date_time, 4);
                atm.tm_year = atoi(buf)-1900;
                date_time += 4;
                break;
            case 1:
                memcpy(buf, date_time, 2);
                atm.tm_mon = atoi(buf) - 1;
                date_time += 2;
                break;
            case 2:
                memcpy(buf, date_time, 2);
                atm.tm_mday = atoi(buf);
                date_time += 2;
                break;
            case 3:
                memcpy(buf, date_time, 2);
                atm.tm_hour = atoi(buf);
                date_time += 2;
                break;
            case 4:
                memcpy(buf, date_time, 2);
                atm.tm_min = atoi(buf);
                date_time += 2;
                break;
            case 5:
                memcpy(buf, date_time, 2);
                atm.tm_sec = atoi(buf);
                date_time += 2;
            }
            ++i;
        }
        time_ = std::mktime(&atm);
    }

    // 构造函数,并将一个time类型的变量赋值给自身
    inline datetime::datetime(const datetime& t) {
        time_ = t.time_;
    }

    // 将一个std::time_t类型的变量赋值给自身
    inline datetime& datetime::operator=(std::time_t t) {
        time_ = t;
        return *this;
    }

    inline datetime& datetime::operator=(const datetime& t) {
        time_ = t.time_;
        return *this;
    }

    // 得到时间(不包括日期)
    inline std::time_t datetime::get_time() const {
        return time_;
    }

    inline bool datetime::valid() const {
        return time_!= -1;
    }

    // 得到年份
    inline int datetime::year() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_year+1900;
    }

    // 得到月份
    inline int datetime::month() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_mon+1;
    }

    // 得到日期
    inline int datetime::day() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_mday;
    }

    // 得到小时
    inline int datetime::hour() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_hour;
    }

    // 得到分钟
    inline int datetime::minute() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_min;
    }

    // 得到秒钟
    inline int datetime::second() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_sec;
    }

    // 得到星期 1=Sun, 2=Mon, ..., 7=Sat
    inline int datetime::get_day_of_week() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_wday+1;
    }

    // 判断2个时间是否相等，相等true,不等false
    inline bool datetime::operator==(const datetime& t) const {
        return time_ == t.time_;
    }

    // 判断2个时间是否不相等，不相等true,相等false
    inline bool datetime::operator!=(const datetime& t) const {
        return time_ != t.time_;
    }

    // 判断另一个时间是否比此时间早
    inline bool datetime::operator<(const datetime& t) const {
        return time_ < t.time_;
    }

    // 判断另一个时间是否比此时间晚
    inline bool datetime::operator>(const datetime& t) const {
        return time_ > t.time_;
    }

    // 判断另一个时间是否比此时间早或等于此时间
    inline bool datetime::operator<=(const datetime& t) const {
        return time_ <= t.time_;
    }

    // 判断另一个时间是否比此时间晚或等于此时间
    inline bool datetime::operator>=(const datetime& t) const {
        return time_ >= t.time_;
    }

    static const int maxTimeBufferSize=128;
    // formatting using "C" strftime
    inline std::string datetime::format(const char * fmt) const {
        char szBuffer[maxTimeBufferSize];
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif

        if( !strftime(szBuffer, maxTimeBufferSize, fmt, &newtime) )
            szBuffer[0] = '\0';
        return szBuffer;

    }

    // 输出日期,格式为"YYYYMMDD"
    inline std::string datetime::date_str() const {
        return format("%Y%m%d");
    }

    // 输出时间,格式为"HHMMSS"
    inline std::string datetime::time_str() const {
        return format("%H%M%S");
    }

    // 输出完整的日期时间，格式为"yyyy-mm-dd hh::mm:ss"
    inline std::string datetime::date_time_str() const {
        return format("%Y-%m-%d %H:%M:%S");
    }
	
    // 输出完整的日期时间，格式为"YYYYMMDDHHMMSS"
    inline std::string datetime::date_time_str2() const {
        return format("%Y%m%d%H%M%S");
    }
}
#endif


