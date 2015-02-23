#ifndef __WUYA_ACE_LOG_H__
#define __WUYA_ACE_LOG_H__

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <cassert>
#include <list>
#include <ace/Thread_Manager.h>
#include <ace/Condition_T.h>
#include <ace/Time_Value.h>
#include <ace/Guard_T.h>
#include <ace/Global_Macros.h>
#include <wuya/datetime.h>
#include <wuya/timespan.h>
#include <wuya/filestat.h>
#include <wuya/fileopt.h>
#include <ace/OS_NS_sys_time.h>

namespace wuya {
    enum LOGLEVEL {
        LOG_ERROR,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG,
    };

    enum NAME_CHANGE_POLICY {
        DAY_CHANGE,
        WEEK_CHANGE,
        MONTH_CHANGE,
        NEVER_CHANGE
    };

    template<class output_type_policy>
    class logger;

    template<class output_type_policy>
    class logstream : public std::ostringstream{
    public:
        explicit logstream();
        ~logstream();
        logstream& val();
    public:
        static bool init(NAME_CHANGE_POLICY np=NEVER_CHANGE, const char* output_name="");
        static bool fini();
    private:
        static logger<output_type_policy>* instance_;
    private:
        logstream( const logstream& );
        const logstream& operator=( const logstream& );
    };

    // some useful operator
    inline std::ostream& thread_id(std::ostream& io) {
        io << std::left << std::setw(8) << ACE_Thread::self();
        return(io);
    }

    inline std::ostream& now(std::ostream& io) {
        io << std::left << std::setw(20) << wuya::datetime::current_time().date_time_str();
        return(io);
    }

    inline ostream& operator<< (ostream& io, LOGLEVEL l){
        switch(l) {
        case LOG_DEBUG:
            io << std::left << std::setw(8) << "DEBUG";break;
        case LOG_ERROR:
            io << std::left << std::setw(8) << "ERROR";break;
        case LOG_INFO:
            io << std::left << std::setw(8) << "INFO";break;
        case LOG_WARN:
            io << std::left << std::setw(8) << "WARN";break;
        }
        return io;
    }

}

// you can redefine as your need
#ifdef _DEBUG
    #define log_debug_cout(message) do{ logdbg_cout << message << std::endl; }while(0)
#else
    #define log_debug_cout(message) do{}while(0)
#endif // _DEBUG

#define LOGSTREAM_COUT logstream<cout_output_type>
#define mylog_cout LOGSTREAM().val()
#define logerr_cout mylog_cout << wuya::LOG_ERROR<< std::setw(20) << now
#define logwarn_cout mylog_cout << wuya::LOG_WARN<< std::setw(20) << now
#define loginfo_cout mylog_cout << wuya::LOG_INFO<< std::setw(20) << now
#define logdbg_cout mylog_cout << wuya::LOG_DEBUG<< std::setw(20) << now

//.............................实现部分.............................//
namespace wuya {
    class name_change_policy{
    public:
        name_change_policy(NAME_CHANGE_POLICY np, const std::string& org_name);
        bool name_changed();
        void reset();
        std::string name();
        void change_name(const wuya::datetime& now);
        void wait();

        NAME_CHANGE_POLICY np_;
        const std::string& org_name_;
        std::string name_;
        wuya::datetime lasttime_;
        volatile bool name_changed_;
        ACE_Thread_Mutex mutex_;
        ACE_Thread_Condition<ACE_Thread_Mutex> condition_;
        ACE_thread_t thread_id_;
    };

    class cout_output_type{
    public:
        bool write(const std::string& msg);
        bool open(const char* name);
        void close();
    };
    class file_output_type{
    public:
        bool write(const std::string& msg);
        bool open(const char* name);
        void close();
    private:
        std::ofstream stream_;
    };
// 与数据库访问方式相关，具体项目中实现 
//  class db_output_type{
//  };

    template<class output_type_policy>
    class logger{
    public:
        logger(NAME_CHANGE_POLICY np, const char* output_name);
        ~logger();
        void add_message(const std::string& msg);

        static void thr_output(void* data);
        static void thr_change_name(void* data);
    private:
        void output();
        void change_name();

        std::string org_name_;
        volatile bool exit_;
        name_change_policy np_;
        output_type_policy op_;

        ACE_Thread_Mutex mutex_;
        ACE_Thread_Condition<ACE_Thread_Mutex> condition_;
        ACE_thread_t thread_id_;
        std::list<std::string> logs_;
    };

    template<class Op_>
    logger<Op_>* logstream<Op_>::instance_;

    template<class Op_>
    bool logstream<Op_>::init(NAME_CHANGE_POLICY np, const char* output_name) {
        if(instance_ == 0) {
            instance_ = new logger<Op_>(np, output_name);
			wuya::filestat fs(output_name);
			make_dir(fs.get_filepath());
        }
        return true;
    }

    template<class Op_>
    bool logstream<Op_>::fini() {
        delete instance_;
        instance_ = 0;
        return true;
    }

    template<class Op_>
    logstream<Op_>::logstream(){
    }

    template<class Op_>
    logstream<Op_>::~logstream() {
        if (instance_)
            instance_->add_message(str());
    }

    template<class Op_>
    logstream<Op_>& logstream<Op_>::val() {
        return *this;
    }

    template<class Op_>
    logger<Op_>::logger(NAME_CHANGE_POLICY np, const char* output_name):
        org_name_(output_name),
        np_(np, org_name_),
        exit_(false),
        condition_(mutex_){
            ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)thr_output, (void*)this, THR_NEW_LWP | THR_JOINABLE
                                                  | THR_INHERIT_SCHED, &thread_id_);
            if(np != NEVER_CHANGE) {
                ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)thr_change_name, (void*)this, THR_NEW_LWP | THR_JOINABLE
                                                      | THR_INHERIT_SCHED, &np_.thread_id_);
            }
            ACE_Thread::yield();
    }

    template<class Op_>
    logger<Op_>::~logger(){
        exit_ = true;
        {
            ACE_GUARD(ACE_Thread_Mutex, guard, mutex_);
            condition_.signal();
            ACE_GUARD(ACE_Thread_Mutex, guard2, np_.mutex_);
            np_.condition_.signal();
        }
        ACE_Thread_Manager::instance()->join(thread_id_);
        if(np_.np_ != NEVER_CHANGE) {
            ACE_Thread_Manager::instance()->join(np_.thread_id_);
        }
        op_.close();
    }

    template<class Op_>
    void logger<Op_>::add_message(const std::string& msg){
        ACE_GUARD(ACE_Thread_Mutex, guard, mutex_);
        logs_.push_back(msg);
        condition_.signal();
    }

    template<class Op_>
    void logger<Op_>::thr_output(void* data){
        ((logger*)data)->output();
    }

    template<class Op_>
    void logger<Op_>::thr_change_name(void* data){
        ((logger*)data)->change_name();
    }

    template<class Op_>
    void logger<Op_>::output(){
        while( true ) {
            ACE_GUARD(ACE_Thread_Mutex, guard, mutex_);
            while( logs_.empty() ) {
                if( exit_ ) {
                    return;
                }
                condition_.wait();
            }
            if( np_.name_changed() ) {
                op_.close();
                op_.open(np_.name().c_str());
                np_.reset();
            }
            while( !logs_.empty() ) {
                op_.write(logs_.front());
                logs_.pop_front();
            }
        }
    }

    template<class Op_>
    void logger<Op_>::change_name(){
        while(!exit_) {
            np_.wait();
            wuya::datetime now = wuya::datetime::current_time();
            np_.change_name(now);
            np_.name_changed_ = true;
            np_.lasttime_ = now;
        }
    }

    inline name_change_policy::name_change_policy(NAME_CHANGE_POLICY np, const std::string& org_name):np_(np),
        org_name_(org_name),condition_(mutex_),name_changed_(true),lasttime_(wuya::datetime::current_time()){
        change_name(wuya::datetime::current_time());
    }
    inline bool name_change_policy::name_changed(){
        return name_changed_;
    }
    inline void name_change_policy::reset(){
        name_changed_ = false;
    }
    inline std::string name_change_policy::name(){
        return name_;
    }
    inline void name_change_policy::change_name(const wuya::datetime& now){
        name_ = org_name_;
        std::string::size_type p = name_.find("$DATE");
        if( p!=std::string::npos ) {
            name_.replace(p, 5, now.date_str());
        }
        p = name_.find("$TIME");
        if( p!=std::string::npos ) {
            name_.replace(p, 5, now.time_str());
        }
    }
    inline void name_change_policy::wait(){
        wuya::datetime time = lasttime_;
        int y,m,d;
        switch( np_ ) {
        case DAY_CHANGE:
            time = time+wuya::timespan(1, 0);
            y = time.year();
            m = time.month();
            d = time.day();
            break;
        case WEEK_CHANGE:
            time = time+wuya::timespan(7, 0);
            y = time.year();
            m = time.month();
            d = time.day();
            break;
        case MONTH_CHANGE:
            y = time.year();
            m = time.month();
            d = time.day();
            if( m==12 ) {
                ++y;
            } else {
                ++m;
            }
        case NEVER_CHANGE:
            return;
        }
        time.set(y, m, d);
        long time_to_sleep = (time-lasttime_).get_total_seconds();
        {
            ACE_GUARD(ACE_Thread_Mutex, guard, mutex_);
            ACE_Time_Value t(time_to_sleep);
            t += ACE_OS::gettimeofday();
            condition_.wait(&t);
        }
    }

    inline bool cout_output_type::write(const std::string& msg){
        std::cout << msg;
        return true;
    }
    inline bool cout_output_type::open(const char* name){
        return true;
    }
    inline void cout_output_type::close(){
    }

    inline bool file_output_type::write(const std::string& msg){
        if(stream_) {
            stream_ << msg;
            stream_.flush();
            return true;
        }
        return false;
    }

    inline bool file_output_type::open(const char* name){
        stream_.clear();
        stream_.open(name, std::ios_base::out|std::ios_base::app);
        return stream_.good();
    }

    inline void file_output_type::close(){
        if(stream_.is_open()) {
            stream_.close();
        }
    }

}
#endif 


