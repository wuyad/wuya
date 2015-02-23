#ifndef __WUYA_GET_OPT_H__
#define __WUYA_GET_OPT_H__

#include <map>
#include <string>

namespace wuya{
	/**
	 * ����main�����Ĳ���
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	class get_opt {
	public:
		get_opt();
		get_opt(int argc, const char** argv);
	public:
		void set(int argc, const char** argv);
		/**
		 * ����ѡ��ָ����Լ��Ƿ�֧�ֳ��ָ���
		 *
		 * @param option_flag
		 *               ѡ��ָ�����һ��Ϊ'-'��'/'
		 * @param has_long_option
		 *               �Ƿ�֧��"--"ѡ��ָ���
		 */
		void set_option(char option_flag, bool has_long_option=true);
	public:
		/**
		 * �Ƿ���ĳ��ѡ��
		 *
		 * @param option ����
		 *
		 * @return
		 */
		bool has_option(char option);
		bool has_option(const char* long_option);
		/**
		 * ȡ��ѡ��������������"-f filename"��filename��Ϊ����������
		 * �������Զ������"-f filename1 filename2"������Ҳ���ܲ�����ĳ��ѡ�����"command filename1 filename2"
		 *
		 * @param option ѡ�Ϊ"*"��ʾ���ز������κ�ѡ��Ĳ���
		 * @param no     ���ֶ������ʱ����noָ����no����������0��ʼ
		 *
		 * @return ����
		 */
		const char* get_option_param(char option='*', int no=0);
		const char* get_option_param(const char* long_option, int no=0);
		/**
		 * ѡ��ж���������ô˷������ز����ĸ���
		 *
		 * @param option ѡ�Ϊ"*"��ʾ���ز������κ�ѡ��Ĳ���
		 *
		 * @return �����ĸ���, -1��ʾѡ�����
		 */
		int get_option_param_size(char option='*');
		int get_option_param_size(const char* long_option);
	protected:
		int argc_;
		const char** argv_;
		bool has_long_option_;
		char option_flag_;

		std::map<std::string, std::pair<int, int> > options_;
	};
}

//.............................ʵ�ֲ���.............................//
namespace wuya{
	inline get_opt::get_opt():argc_(0), argv_(0), has_long_option_(true),
	option_flag_('-') {
	}

	inline get_opt::get_opt(int argc, const char** argv):argc_(argc),argv_(argv),
	has_long_option_(true),option_flag_('-') {
		set(argc, argv);
	}

	inline void get_opt::set(int argc, const char** argv) {
		const char* prov_option = 0;
		int prov_pos = 0;
		bool prov_long_flag = false;
		int param_size = 0;
		for (int i = 1; i < argc; i++) {
			const char* p = argv[i];
			int flag = 0;
			if (p[0]=='-' && p[1]=='-') {
				flag = 2;
				p += 2;
			} else if (p[0] == option_flag_) {
				flag = 1;
				++p;
			}
			if (flag) {
				if (prov_option != 0) {
					if (prov_long_flag) {
						options_[prov_option] = std::make_pair(param_size, prov_pos+1);
					} else {
						char tmp[2];
						tmp[1] = '\0';
						while ( (tmp[0]=*prov_option++) != '\0') {
							options_[tmp] = std::make_pair(param_size, prov_pos+1);
						}
					}
				} else if (param_size != 0) {
					options_["*"] = std::make_pair(param_size, prov_pos+1);
				}
				prov_option = p;
				prov_pos = i;
				param_size = 0;
				prov_long_flag = (flag==2);
			} else {
				++param_size;
			}
		}
		if (prov_option != 0) {
			if (prov_long_flag) {
				options_[prov_option] = std::make_pair(param_size, prov_pos+1);
			} else {
				char tmp[2];
				tmp[1] = '\0';
				while ( (tmp[0]=*prov_option++) != '\0') {
					options_[tmp] = std::make_pair(param_size, prov_pos+1);
				}
			}
		}
		if (param_size != 0 && options_.find("*") == options_.end()) {
			options_["*"] = std::make_pair(param_size, prov_pos+1);
		}
	}

	inline void get_opt::set_option(char option_flag, bool has_long_option) {
		option_flag_ = option_flag;
		has_long_option_ = has_long_option;
	}

	inline bool get_opt::has_option(char option) {
		char tmp[2];
		tmp[0] = option;
		tmp[1] = '\0';
		return has_option(tmp);
	}

	inline bool get_opt::has_option(const char* long_option) {
		return options_.find(long_option) != options_.end();
	}

	inline const char* get_opt::get_option_param(char option, int no) {
		char tmp[2];
		tmp[0] = option;
		tmp[1] = '\0';
		return get_option_param(tmp, no);
	}

	inline const char* get_opt::get_option_param(const char* long_option, int no) {
		std::map<std::string, std::pair<int, int> >::const_iterator iter;
		if ((iter=options_.find(long_option)) != options_.end()) {
			int size = (*iter).second.first;
			if (no<size) {
				return argv_[(*iter).second.second+no];
			}
		}
		return 0;
	}

	inline int get_opt::get_option_param_size(char option) {
		char tmp[2];
		tmp[0] = option;
		tmp[1] = '\0';
		return get_option_param_size(tmp);
	}

	inline int get_opt::get_option_param_size(const char* long_option) {
		std::map<std::string, std::pair<int, int> >::const_iterator iter;
		if ((iter=options_.find(long_option)) != options_.end()) {
			return(*iter).second.first;
		}
		return -1;
	}
}

#endif // __GET_OPT_H__


