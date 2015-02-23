#ifndef __WUYA_SOCKET_H__
#define __WUYA_SOCKET_H__

#if defined(WIN32)||defined(_WIN32)
	#include <winsock2.h>
typedef SOCKET socket_type;
#else
	#include <netdb.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
typedef int socket_type;
#endif
namespace wuya{
	bool sock_init();
	void sock_fini();

	class ip_addr {
	public:
		ip_addr();
		ip_addr(const sockaddr_in *, int len);
		ip_addr(unsigned short port_number, const char host_name[]);
		explicit ip_addr(const char address[]);
		explicit ip_addr(unsigned short port_number, unsigned long ip_addr=INADDR_ANY);

		int set(const sockaddr_in *, int len);
		int set(unsigned short port_number, const char host_name[]);
		int set(const char address[]);
		int set(unsigned short port_number, unsigned long ip_addr=INADDR_ANY);

		unsigned short get_port_number() const;
		const char *get_host_addr () const;
		void * get_addr() const;
	protected:
		void reset();
		sockaddr_in  inet_addr_;
	};

	class sock_stream {
	public:
		int recv(void *buf, int n);
		int send(const void *buf, int n);
		int recv_n(void *buf, int n);
		int send_n(const void *buf, int n);
	public:
		int close();
		int get_local_addr (ip_addr& addr) const;
		int get_remote_addr (ip_addr& addr) const;
		socket_type get_handler();
		void set_handler(socket_type h);
	protected:
		socket_type sock_;
	};

	class sock_connector {
	public:
		sock_connector();
		sock_connector(sock_stream &new_stream, const ip_addr &remote_sap);
		int connect(sock_stream &new_stream, const ip_addr &remote_sap);
	};

	class sock_acceptor {
	public:
		sock_acceptor();
		sock_acceptor(const ip_addr &local_sap, int reuse_addr=0);
		int open (const ip_addr &local_sap, int reuse_addr=0);
		int accept (sock_stream &new_stream);
	protected:
		socket_type sock_;
	};


//.............................实现部分.............................//
	inline bool sock_init() {
#if defined(WIN32)||defined(_WIN32)
		WSADATA wsadata;
		if (WSAStartup(MAKEWORD(2, 0), &wsadata) == 0) {
			return true;
		} else {
			return false;
		}
#else
		return true;
#endif
	}

	inline void sock_fini() {
#if defined(WIN32)||defined(_WIN32)
		WSACleanup();
#endif
	}

	inline ip_addr::ip_addr() {
		reset();
	}
	inline void ip_addr::reset() {
		memset (&this->inet_addr_, 0, sizeof (this->inet_addr_));
		this->inet_addr_.sin_family = AF_INET;
	}
	inline ip_addr::ip_addr(const sockaddr_in * addr, int len) {
		this->reset ();
		this->set (addr, len);
	}
	inline ip_addr::ip_addr(unsigned short port_number, const char host_name[]) {
		this->reset ();
		this->set (port_number, host_name);
	}
	inline ip_addr::ip_addr(const char address[]) {
		this->reset ();
		this->set (address);
	}
	inline ip_addr::ip_addr(unsigned short port_number, unsigned long ip_addr) {
		this->reset ();
		this->set (port_number, ip_addr);
	}

	inline int ip_addr::set(const sockaddr_in * addr, int len) {
		if (addr->sin_family == AF_INET) {
			int maxlen = static_cast<int> (sizeof (this->inet_addr_));
			if (len > maxlen)
				len = maxlen;
			memcpy (&this->inet_addr_, addr, len);
			return 0;
		}
		return -1;
	}
	inline int ip_addr::set(unsigned short port_number, const char host_name[]) {
		if (host_name == 0) {
			return -1;
		}
		unsigned long addr = ::inet_addr(host_name);
		if (addr != INADDR_NONE) {
			inet_addr_.sin_addr.s_addr = addr;
		} else {
			hostent *hp = ::gethostbyname(host_name);
			if (hp == NULL || hp->h_addrtype != AF_INET) {
				return -1;
			}
			memcpy(&(inet_addr_.sin_addr), hp->h_addr_list[0], hp->h_length);
		}
		inet_addr_.sin_port = htons((unsigned short)port_number);
		return 0;
	}
	inline int ip_addr::set(const char address[]) {
		char* buf = new char[strlen(address)+1];
		strcpy(buf, address);
		char* p = strrchr(buf, ':');
		unsigned short port = 0;
		if (p != 0) {
			port = atoi(p+1);
			*p = '\0';
		}
		set(port, buf);
		delete [] buf;
		return 0;
	}
	inline int ip_addr::set(unsigned short port_number, unsigned long ip_addr) {
		inet_addr_.sin_port = htons((unsigned short)port_number);
		ip_addr = htonl(ip_addr);
		memcpy (&this->inet_addr_.sin_addr,&ip_addr,sizeof ip_addr);
		return 0;
	}

	inline unsigned short ip_addr::get_port_number() const {
		return ntohs (this->inet_addr_.sin_port);
	}
	inline const char *ip_addr::get_host_addr () const {
		return inet_ntoa(this->inet_addr_.sin_addr);
	}
	inline void *ip_addr::get_addr () const {
		return(void*)&inet_addr_;
	}

	inline int sock_stream::recv(void *buf, int n) {
		return ::recv(sock_, (char*)buf, n, 0);
	}
	inline int sock_stream::send(const void *buf, int n) {
		return ::send(sock_, (const char*)buf, n, 0);
	}
	inline int sock_stream::recv_n(void *buf, int n) {
		int left = n;
		int n_read;
		while (left !=0 && (n_read = ::recv(sock_, (char*)buf+(n-left), left, 0)) != SOCKET_ERROR) {
			left -= n_read;
		}
		return n - left;
	}
	inline int sock_stream::send_n(const void *buf, int n) {
		int left = n;
		int n_read;
		while (left !=0 && (n_read = ::send(sock_, (char*)buf+(n-left), left, 0)) != SOCKET_ERROR) {
			left -= n_read;
		}
		return n - left;
	}
	inline int sock_stream::close() {
#if defined(WIN32)||defined(_WIN32)
		if (::closesocket(sock_) == SOCKET_ERROR) {
			return -1;
		} else {
			return 0;
		}
#else
		if (::close(sock_) == SOCKET_ERROR) {
			return -1;
		} else {
			return 0;
		}
#endif
	}
	inline int sock_stream::get_local_addr (ip_addr& addr) const {
		sockaddr *paddr = reinterpret_cast<sockaddr *> (addr.get_addr ());
#ifdef SOCKLEN_T
		socklen_t len;
#else
		int len;
#endif
		if (::getsockname (sock_, paddr, &len) == SOCKET_ERROR)
			return -1;
		return 0;
	}
	inline int sock_stream::get_remote_addr (ip_addr& addr) const {
		sockaddr *paddr = reinterpret_cast<sockaddr *> (addr.get_addr ());
#ifdef SOCKLEN_T
		socklen_t len;
#else
		int len;
#endif
		if (::getpeername (sock_, paddr, &len) == SOCKET_ERROR)
			return -1;
		return 0;
	}
	inline socket_type sock_stream::get_handler() {
		return sock_;
	}
	inline void sock_stream::set_handler(socket_type h) {
		sock_ = h;
	}

	inline sock_connector::sock_connector() {

	}
	inline sock_connector::sock_connector(sock_stream &new_stream, const ip_addr &remote_sap) {
		connect(new_stream, remote_sap);
	}
	inline int sock_connector::connect(sock_stream &new_stream, const ip_addr &remote_sap) {
		socket_type sock = ::socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			return -1;
		}
		new_stream.set_handler(sock);
		if (::connect(new_stream.get_handler(), (sockaddr*)remote_sap.get_addr(), sizeof(sockaddr)) == SOCKET_ERROR) {
			return -1;
		}
		return 0;
	}

	inline sock_acceptor::sock_acceptor() {
	}
	inline sock_acceptor::sock_acceptor(const ip_addr &local_sap, int reuse_addr) {
		open(local_sap, reuse_addr);
	}
	inline int sock_acceptor::open (const ip_addr &local_sap, int reuse_addr) {
		socket_type sock = ::socket(AF_INET, SOCK_STREAM, 0);
		if (sock_ == INVALID_SOCKET) {
			return -1;
		}
		sock_ = sock;
		if (reuse_addr) {
			int one = 1;
			setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char*)&one, sizeof one);
		}
		if (bind (sock_, (sockaddr*)local_sap.get_addr(), sizeof(sockaddr)) == SOCKET_ERROR) {
			return -1;
		}
		if (listen(sock_, 100) == SOCKET_ERROR) {
			return -1;
		}
		return 0;
	}
	inline int sock_acceptor::accept (sock_stream &new_stream) {
		sockaddr_in from;
#ifdef SOCKLEN_T
		socklen_t len;
#else
		int len;
#endif
		len = sizeof(from);
		memset(&from, 0, len);

		socket_type newsocket = ::accept(sock_, (sockaddr*)&from, &len);
		if (newsocket == INVALID_SOCKET) {
			return -1;
		}
		new_stream.set_handler(newsocket);
		return 0;
	}
}

#endif


