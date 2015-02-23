#ifndef __WUYA_ACE_SOCKET_H__
#define __WUYA_ACE_SOCKET_H__

#include <ace/OS_NS_sys_socket.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Acceptor.h>

namespace wuya{
	namespace ace{
		bool sock_init();
		void sock_fini();

		typedef ACE_INET_Addr ip_addr;

		typedef ACE_SOCK_Stream sock_stream;

		typedef ACE_SOCK_Connector sock_connector;

		typedef ACE_SOCK_Acceptor sock_acceptor;


//.............................实现部分.............................//
		inline bool sock_init() {
			return ACE_OS::socket_init();
		}

		inline void sock_fini() {
			ACE_OS::socket_fini();
		}
	}
}

#endif


