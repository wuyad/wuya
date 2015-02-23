#ifndef __WUYA_DATETIME_H__
#define __WUYA_DATETIME_H__

#include <ctime>
#include <ostream>
#include <string>

namespace wuya {
    /**
     * ʱ���࣬����һ��ʱ���
     *
     * @author wuya
     * @version 1.0.0
     */
    class datetime {
    public:
        datetime();
        // ���캯��������һ��std::time_t���͵ı�����ֵ������
        datetime(std::time_t t);
        // ���캯��������ֵΪ�꣬��(1-12)����(1-31)��ʱ(0-23)����(0-59)����(0-59)��
        // �����������������ʱ��
        datetime(int year, int month, int day, int hour=0, int minu=0, int sec=0, int dst=0);
        // ���캯��������Ϊ"YYYYMMDDHHMMSS"��"YYYYMMDD"���ַ����������ַ�������ʱ��
        datetime(const char* date_time);
        // ���캯��,����һ��time���͵ı�����ֵ������
        datetime(const datetime& t);
    public:
        // �ж��Ƿ�����
        static bool is_leap_year( int year );
        void set(std::time_t t);
        void set(int year, int month, int day=1, int hour=0, int minu=0, int sec=0, int dst=0);
        void set(const char* date_time);
        // formatting using "C" strftime
        std::string format(const char* fmt) const;
        // �������,��ʽΪ"YYYYMMDD"
        std::string date_str() const;
        // ���ʱ��,��ʽΪ"HHMMSS"
        std::string time_str() const;
        // �������������ʱ�䣬��ʽΪ"yyyy-mm-dd hh::mm:ss"
        std::string date_time_str() const;
        // �������������ʱ�䣬��ʽΪ"YYYYMMDDHHMMSS"
        std::string date_time_str2() const;
    public:
        // ��ȡ��ǰϵͳʱ��
        static datetime current_time();
        // �õ�ʱ��(����������)
        std::time_t get_time() const;
        // �Ƿ���Ч
        bool valid() const;
        // �õ����
        int year() const;
        // �õ��·ݣ�1-12
        int month() const;
        // �õ����ڣ�1-31
        int day() const;
        // �õ�Сʱ��0-23
        int hour() const;
        // �õ����ӣ�0-59
        int minute() const;
        // �õ����ӣ�0-59
        int second() const;
        // �õ����� 1=Sun, 2=Mon, ..., 7=Sat
        int get_day_of_week() const;
    public:
        // ��һ��std::time_t���͵ı�����ֵ������
        datetime& operator=(std::time_t t);
        datetime& operator=(const datetime& t);
        // �ж�2��ʱ���Ƿ���ȣ����true,����false
        bool operator==(const datetime& t) const;
        // �ж�2��ʱ���Ƿ���ȣ������true,���false
        bool operator!=(const datetime& t) const;
        // �ж���һ��ʱ���Ƿ�ȴ�ʱ����
        bool operator<(const datetime& t) const;
        // �ж���һ��ʱ���Ƿ�ȴ�ʱ����
        bool operator>(const datetime& t) const;
        // �ж���һ��ʱ���Ƿ�ȴ�ʱ�������ڴ�ʱ��
        bool operator<=(const datetime& t) const;
        // �ж���һ��ʱ���Ƿ�ȴ�ʱ�������ڴ�ʱ��
        bool operator>=(const datetime& t) const;
    protected:
        // ��Ա��������Ŵ�ʱ�������ʱ����Ϣ
        std::time_t time_;
    };
}

//.............................ʵ�ֲ���.............................//
namespace wuya {
    // ��ȡ��ǰϵͳʱ��
    inline datetime datetime::current_time() {
        std::time_t temp;
        return datetime(std::time(&temp));
    }

    // �ж��Ƿ�����
    inline bool datetime::is_leap_year( int year ) {
        return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
    }

    inline datetime::datetime():time_(0) {
    }

    // ���캯��������һ��std::time_t���͵ı�����ֵ������
    inline datetime::datetime(std::time_t t) {
        set(t);
    }

    // ���캯��������ֵΪ�꣬�£��գ�ʱ���֣��룬�����������������ʱ��
    inline datetime::datetime(int year, int month, int day, int hour, int minu, int sec, int dst) {
        set(year, month, day, hour, minu, sec, dst);
    }

    // ���캯��������Ϊ"YYYYMMDDHHMMSS"��"YYYYMMDD"���ַ����������ַ�������ʱ��
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

    // ���캯��,����һ��time���͵ı�����ֵ������
    inline datetime::datetime(const datetime& t) {
        time_ = t.time_;
    }

    // ��һ��std::time_t���͵ı�����ֵ������
    inline datetime& datetime::operator=(std::time_t t) {
        time_ = t;
        return *this;
    }

    inline datetime& datetime::operator=(const datetime& t) {
        time_ = t.time_;
        return *this;
    }

    // �õ�ʱ��(����������)
    inline std::time_t datetime::get_time() const {
        return time_;
    }

    inline bool datetime::valid() const {
        return time_!= -1;
    }

    // �õ����
    inline int datetime::year() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_year+1900;
    }

    // �õ��·�
    inline int datetime::month() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_mon+1;
    }

    // �õ�����
    inline int datetime::day() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_mday;
    }

    // �õ�Сʱ
    inline int datetime::hour() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_hour;
    }

    // �õ�����
    inline int datetime::minute() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_min;
    }

    // �õ�����
    inline int datetime::second() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_sec;
    }

    // �õ����� 1=Sun, 2=Mon, ..., 7=Sat
    inline int datetime::get_day_of_week() const {
        struct tm newtime;
#ifdef _MSC_VER
        localtime_s(&newtime, &time_);
#else
        localtime_r(&time_, &newtime);
#endif
        return newtime.tm_wday+1;
    }

    // �ж�2��ʱ���Ƿ���ȣ����true,����false
    inline bool datetime::operator==(const datetime& t) const {
        return time_ == t.time_;
    }

    // �ж�2��ʱ���Ƿ���ȣ������true,���false
    inline bool datetime::operator!=(const datetime& t) const {
        return time_ != t.time_;
    }

    // �ж���һ��ʱ���Ƿ�ȴ�ʱ����
    inline bool datetime::operator<(const datetime& t) const {
        return time_ < t.time_;
    }

    // �ж���һ��ʱ���Ƿ�ȴ�ʱ����
    inline bool datetime::operator>(const datetime& t) const {
        return time_ > t.time_;
    }

    // �ж���һ��ʱ���Ƿ�ȴ�ʱ�������ڴ�ʱ��
    inline bool datetime::operator<=(const datetime& t) const {
        return time_ <= t.time_;
    }

    // �ж���һ��ʱ���Ƿ�ȴ�ʱ�������ڴ�ʱ��
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

    // �������,��ʽΪ"YYYYMMDD"
    inline std::string datetime::date_str() const {
        return format("%Y%m%d");
    }

    // ���ʱ��,��ʽΪ"HHMMSS"
    inline std::string datetime::time_str() const {
        return format("%H%M%S");
    }

    // �������������ʱ�䣬��ʽΪ"yyyy-mm-dd hh::mm:ss"
    inline std::string datetime::date_time_str() const {
        return format("%Y-%m-%d %H:%M:%S");
    }
	
    // �������������ʱ�䣬��ʽΪ"YYYYMMDDHHMMSS"
    inline std::string datetime::date_time_str2() const {
        return format("%Y%m%d%H%M%S");
    }
}
#endif


