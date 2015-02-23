#ifndef __WUYA_COM_PORT_H__
#define __WUYA_COM_PORT_H__

#include <string>
#include <iostream>

#if defined(WIN32)||defined(_WIN32)
	#define _WINSOCKAPI_
	#include <windows.h>

	#define COM_HANDLE HANDLE
#else
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>
	#define COM_HANDLE int
	#define NOPARITY 0
	#define ONESTOPBIT 0
	#define INVALID_HANDLE_VALUE -1
#endif 

namespace wuya
{
	class com_port
	{
	public:
		com_port();
		com_port(const char* name, int baudrate=9600, int parity=NOPARITY, 
				 int byte_size=8, int stopbits=ONESTOPBIT);
		bool open(const char* name, int baudrate=9600, int parity=NOPARITY, 
				  int byte_size=8, int stopbits=ONESTOPBIT);
		bool close();
		bool write(const std::string& data);
		bool read(std::string& data);
		bool is_opened();
		~com_port();
	protected:
		COM_HANDLE _handle;
		bool _opened;
	private:
		com_port(const com_port& src);
		com_port& operator=(const com_port& src);
	};
}

//.............................ʵ�ֲ���.............................//
namespace wuya
{
	inline com_port::com_port() : _handle(INVALID_HANDLE_VALUE), _opened(false)
	{
	}

	inline com_port::~com_port()
	{
		close();
	}

	inline bool com_port::is_opened()
	{
		return _opened;
	}


	inline com_port::com_port(const char* name, int baudrate, int parity, 
					   int byte_size, int stopbits)
	{
		open(name, baudrate, parity, byte_size, stopbits);
	}

#if defined(WIN32)||defined(_WIN32)

	inline bool com_port::open(const char* name, int baudrate, int parity, 
						int byte_size, int stopbits)
	{
		_opened = false;
		DCB dcb;		// ���ڿ��ƿ�
		COMMTIMEOUTS timeouts = {	// ���ڳ�ʱ���Ʋ���
			100,				// ���ַ������ʱʱ��: 100 ms
			1,					// ������ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
			500,				// ������(�����)����ʱʱ��: 500 ms
			1,					// д����ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
			100};				// ������(�����)д��ʱʱ��: 100 ms
// 	COMMTIMEOUTS timeouts = {	// ���ڳ�ʱ���Ʋ���
// 		100,				// ���ַ������ʱʱ��: 100 ms
// 		100,					// ������ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
// 		0,				// ������(�����)����ʱʱ��: 500 ms
// 		100,					// д����ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
// 		0};				// ������(�����)д��ʱʱ��: 100 ms
// 	COMMTIMEOUTS timeouts = {	// ���ڳ�ʱ���Ʋ���
// 		0,				// ���ַ������ʱʱ��: 100 ms
// 		0,					// ������ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
// 		0,				// ������(�����)����ʱʱ��: 500 ms
// 		0,					// д����ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
// 		1000};				// ������(�����)д��ʱʱ��: 100 ms
// 	COMMTIMEOUTS timeouts = {	// ���ڳ�ʱ���Ʋ���
// 		100,				// ���ַ������ʱʱ��: 100 ms
// 		1,					// ������ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
// 		100,				// ������(�����)����ʱʱ��: 500 ms
// 		0,					// д����ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
// 		0};				// ������(�����)д��ʱʱ��: 100 ms

		_handle = CreateFile(name,	// �������ƻ��豸·��
							 GENERIC_READ | GENERIC_WRITE,	 // ��д��ʽ
							 0,				 // ����ʽ����ռ
							 NULL,			 // Ĭ�ϵİ�ȫ������
							 OPEN_EXISTING,	 // ������ʽ
							 0,				 // ���������ļ�����
							 NULL);			 // �������ģ���ļ�

		if (_handle == INVALID_HANDLE_VALUE)
		{
			return _opened;		// �򿪴���ʧ��
		}
		GetCommState(_handle, &dcb);		// ȡDCB

		dcb.BaudRate = baudrate;
		dcb.ByteSize = byte_size;
		dcb.Parity = parity;
		dcb.StopBits = stopbits;

		SetCommState(_handle, &dcb);		// ����DCB
		SetupComm(_handle, 4096, 1024);	// �������������������С
		SetCommTimeouts(_handle, &timeouts);	// ���ó�ʱ
		_opened = true;
		return _opened;
	}

	inline bool com_port::close()
	{
		_opened = false;
		return CloseHandle(_handle)==TRUE;
	}

	inline bool com_port::read(std::string& data)
	{
		if (_opened == false)
		{
			return false;
		}
		DWORD num = 1;
		char buf[256];
		ReadFile(_handle, buf, (DWORD)256, &num, 0);
		if (num>0)
		{
			data.append(buf, num);
		}
		return num>0;
	}

	inline bool com_port::write(const std::string& data)
	{
		if (_opened == false)
		{
			return false;
		}
		DWORD num;
		size_t src_len = data.length();
		size_t totle = 0;
		bool ret = false;
		while (totle < src_len)
		{
			if (WriteFile(_handle, data.c_str()+totle, (DWORD)(src_len-totle), &num, 0) == 0)
			{
				break;
			}
			ret = true;
			totle += num;
		}
		return ret;
	}

#else

	inline bool com_port::open(const char* name, int baudrate, int parity, 
						int byte_size, int stopbits)
	{
		_opened = false;
		_handle = ::open(name, O_RDWR|O_NOCTTY|O_NONBLOCK);
		if (_handle == -1)
		{
			return _opened;
		}
// 	fcntl(_handle, F_SETFL, O_ASYNC|O_NONBLOCK);
		tcflag_t tc_baudrate;
		tcflag_t tc_databits;
		tcflag_t tc_stopbits;
		tcflag_t tc_checkparity;

		/* get the propr baudrate */
		switch (baudrate)
		{
		case 75:
			tc_baudrate=B75;
			break;
		case 110:
			tc_baudrate=B110;
			break;
		case 150:
			tc_baudrate=B150;
			break;
		case 300:
			tc_baudrate=B300;
			break;
		case 600:
			tc_baudrate=B600;
			break;
		case 1200:
			tc_baudrate=B1200;
			break;
		case 2400:
			tc_baudrate=B2400;
			break;
		case 4800:
			tc_baudrate=B4800;
			break;
		case 9600:
			tc_baudrate=B9600;
			break;
		case 19200:
			tc_baudrate=B19200;
			break;
		case 38400:
			tc_baudrate=B38400;
			break;
		case 57600:
			tc_baudrate=B57600;
			break;
		case 115200:
			tc_baudrate=B115200;
			break;
		default:
			tc_baudrate=B9600;
		}

		switch (byte_size)
		{
		case 7:
			tc_databits=CS7;
			break;
		case 8:
			tc_databits=CS8;
			break;
		default:
			tc_databits=CS8;
		}

		/* parity, */
		switch (parity)
		{
		case 0:
			tc_checkparity=0;
			break;
		case 1:
			tc_checkparity=PARENB|PARODD;
			break;
		case 2:
			tc_checkparity=PARENB;
			break;
		default:
			tc_checkparity=0;
		}

		/* and stop bits */
		switch (stopbits)
		{
		case 1:
			tc_stopbits=0;
			break;
		case 2:
			tc_stopbits=CSTOPB;
			break;
		default:
			tc_stopbits=0;
		}

		struct termios tio;
		memset(&tio, 0, sizeof(tio));
		tio.c_cflag=tc_baudrate|tc_databits|tc_checkparity|tc_stopbits|CLOCAL|CREAD;
		tio.c_iflag=IGNPAR;
		tio.c_oflag=0;
		tio.c_lflag=0;
		tio.c_cc[VMIN]=1;
		tio.c_cc[VTIME]=1;

		/* we flush the port */
		tcflush(_handle,TCOFLUSH);
		tcflush(_handle,TCIFLUSH);

		/* we send new config to the port */
		tcsetattr(_handle,TCSANOW,&tio);

		_opened = true;
		return true;
	}

	inline bool com_port::close()
	{
		_opened = false;
		return ::close(_handle)==0;
	}

	inline bool com_port::read(std::string& data)
	{
		if (_opened == false)
		{
			return false;
		}
		int num = 0;
		char buf[256];
		num = ::read(_handle, buf, 256);
		if (num>0)
		{
			data.append(buf, num);
			return true;
		}
		return false;
	}

	inline bool com_port::write(const std::string& data)
	{
		if (_opened == false)
		{
			return false;
		}
		return ::write(_handle, data.c_str(), data.length())>0;
	}

#endif 

}


#endif // __WUYA_COM_PORT_H__

