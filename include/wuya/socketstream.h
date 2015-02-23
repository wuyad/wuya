#ifndef __WUYA_SOCKETSTREAM_H__
#define __WUYA_SOCKETSTREAM_H__

#include <iostream>
#include <streambuf>
#include <iosfwd>

namespace wuya{
	template <class sock_streamT, class charT, class traits = std::char_traits<charT> >
	class socketstreambuf;

	template <class sock_streamT, class charT = char, class traits = std::char_traits<charT> >
	class socketstream :public std::basic_iostream<charT, traits> {
	public:
		explicit socketstream(sock_streamT& sock, bool takeowner = false);
	private:
		socketstream(const socketstream& );
		socketstream& operator=(const socketstream& );
		socketstreambuf<sock_streamT, charT, traits> streambuf_;
	};

//.............................实现部分.............................//
#define BUFSIZE_DEFAULT	2048

	template <class sock_streamT, class charT, class traits >
	class socketstreambuf : public std::basic_streambuf<charT, traits> {
		typedef std::basic_streambuf<charT, traits> sbuf_type;
		typedef typename sbuf_type::int_type int_type;
		typedef charT char_type;
	public:
		explicit socketstreambuf(sock_streamT& sock, bool takeowner = false,
								 std::streamsize bufsize = BUFSIZE_DEFAULT);
		~socketstreambuf();
	protected:
		std::basic_streambuf<charT, traits>* setbuf(char_type *s, std::streamsize n);
		void _flush();
		int_type overflow(int_type c = traits::eof());
		int sync();
		int_type underflow();
	private:
		socketstreambuf(const socketstreambuf& );
		socketstreambuf& operator=(const socketstreambuf& );

		sock_streamT& rsocket_;
		bool ownsocket_;
		char_type* inbuf_;
		char_type* outbuf_;
		std::streamsize bufsize_;
		size_t remained_;
		char_type remainedchar_;
		bool ownbuffers_;
	}; // socketstreambuf

	template <class sock_streamT, class charT, class traits>
	socketstreambuf<sock_streamT, charT, traits>::socketstreambuf (
																  sock_streamT& sock,
																  bool takeowner,
																  std::streamsize bufsize
																  )
	:
	rsocket_(sock),
	ownsocket_(takeowner),
	inbuf_(0),
	outbuf_(0),
	bufsize_(bufsize),
	remained_(0),
	ownbuffers_(false) {
	}

	template <class sock_streamT, class charT, class traits>
	socketstreambuf<sock_streamT, charT, traits>::~socketstreambuf() {
		_flush();

		if (ownbuffers_) {
			delete [] inbuf_;
			delete [] outbuf_;
		}

		if (ownsocket_)	rsocket_.close();
	}

	template <class sock_streamT, class charT, class traits>
	std::basic_streambuf<charT, traits>* socketstreambuf<sock_streamT, charT, traits>::setbuf (char_type *s, std::streamsize n) {
		if (gptr() == 0) {
			setg (s, s + n, s + n);
			setp (s, s + n);
			inbuf_ = s;
			outbuf_ = s;
			bufsize_ = n;
			ownbuffers_ = false;
		}
		return this;
	}

	template <class sock_streamT, class charT, class traits>
	void socketstreambuf<sock_streamT, charT, traits>::_flush() {
		rsocket_.send(outbuf_, (int)(pptr() - outbuf_) * sizeof(char_type));
	}

	template <class sock_streamT, class charT, class traits>
	typename socketstreambuf<sock_streamT, charT, traits>::int_type
	socketstreambuf<sock_streamT, charT, traits>::overflow(int_type c) {
		// this method is supposed to flush the put area of the buffer
		// to the I/O device

		// if the buffer was not already allocated nor set by user,
		// do it just now
		if (pptr() == 0) {
			outbuf_ = new char_type[bufsize_];
			ownbuffers_ = true;
		} else {
			_flush();
		}

		setp(outbuf_, outbuf_ + bufsize_);
		if (c != traits::eof()) {
			sputc(traits::to_char_type(c));
		}
		return 0;
	}

	template <class sock_streamT, class charT, class traits>
	int socketstreambuf<sock_streamT, charT, traits>::sync() {
		// just flush the put area
		_flush();
		setp (outbuf_, outbuf_ + bufsize_);
		return 0;
	}

	template <class sock_streamT, class charT, class traits>
	typename socketstreambuf<sock_streamT, charT, traits>::int_type
	socketstreambuf<sock_streamT, charT, traits>::underflow() {
		// this method is supposed to read some bytes from the I/O device

		// if the buffer was not already allocated nor set by user,
		// do it just now
		if (gptr() == 0) {
			inbuf_ = new char_type[bufsize_];
			ownbuffers_ = true;
		}

		if (remained_ != 0)
			inbuf_[0] = remainedchar_;

		size_t readn = rsocket_.recv (
									 static_cast<char*>(inbuf_) + remained_,
									 (int)(bufsize_ * sizeof(char_type) - remained_)
									 );

		// if (readn == 0 && remained_ != 0)
		// error - there is not enough bytes for completing
		// the last character before the end of the stream
		// - this can mean error on the remote end
		if (readn == 0)	 return traits::eof();

		size_t totalbytes = readn + remained_;
		setg (inbuf_, inbuf_, inbuf_ + totalbytes / sizeof(char_type));

		remained_ = totalbytes % sizeof(char_type);
		if (remained_ != 0) {
			remainedchar_ = inbuf_[totalbytes / sizeof(char_type)];
		}
		return sgetc();
	}

	template <class sock_streamT, class charT, class traits>
	socketstream<sock_streamT, charT, traits>::socketstream (
											  sock_streamT& sock,
											  bool takeowner
											  )
	:streambuf_(sock, takeowner),
	std::basic_iostream<charT, traits>(&streambuf_) {
	}

}

#endif


