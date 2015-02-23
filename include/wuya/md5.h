#ifndef __WUYA_MD5_H__
#define __WUYA_MD5_H__

#include <cstring>
#include <cstdio>

namespace
{
	struct MD5_CTX;
}
namespace wuya
{

	class md5
	{
	public:
		md5();
		md5(const char* str);
		~md5();

		void init(const char* str);
		void do_digest();

		const unsigned char* get_result();
		const char* get_sorce();

		char* get_hex_result();
	private:
		md5(const md5&);
		md5& operator=(const md5&);

		MD5_CTX* context_;
		unsigned char result_[17];
		char hex_result_[16*2+1];
		const char* source_;
	};
}


//.............................实现部分.............................//

namespace
{
	typedef unsigned long int uint32_t;
	typedef unsigned char  uint8_t;

/* Define the state of the MD5 Algorithm. */
	struct MD5_CTX
	{
		uint32_t count[2];	/* message length in bits, lsw first */
		uint32_t state[4];	/* digest buffer */
		uint8_t buffer[64];	/* accumulate block */
	};

	void    MD5_Init(MD5_CTX *pms);
	void    MD5_Update(MD5_CTX *pms, const uint8_t *data, size_t nbytes);
	void    MD5_Finish(MD5_CTX *pms, uint8_t *digest);



#define MD5_BLOCK_LENGTH		64
#define MD5_DIGEST_LENGTH		16
#define MD5_DIGEST_STRING_LENGTH	(MD5_DIGEST_LENGTH * 2 + 1)

#ifdef T_MASK
#undef T_MASK
#endif
#define T_MASK ((uint32_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)


	static void md5_process(MD5_CTX *pms, const uint8_t *data) /*[64]*/
	{
		uint32_t
		a = pms->state[0], b = pms->state[1],
							   c = pms->state[2], d = pms->state[3];
		uint32_t t;

#ifdef WORDS_BIGENDIAN

		/*
		 * On big-endian machines, we must arrange the bytes in the right
		 * order.  (This also works on machines of unknown byte order.)
		 */
		uint32_t X[16];
		const uint8_t *xp = data;
		int i;

		for (i = 0; i < 16; ++i, xp += 4)
			X[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);

#else

		/*
		 * On little-endian machines, we can process properly aligned data
		 * without copying it.
		 */
		uint32_t xbuf[16];
		const uint32_t *X;

		if (!((data - (const uint8_t *)0) & 3))
		{
			/* data are properly aligned */
			X = (const uint32_t *)data;
		}
		else
		{
			/* not aligned */
			std::memcpy(xbuf, data, 64);
			X = xbuf;
		}
#endif

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

		/* Round 1. */
		/* Let [abcd k s i] denote the operation
		   a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + F(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  0,  7,  T1);
		SET(d, a, b, c,  1, 12,  T2);
		SET(c, d, a, b,  2, 17,  T3);
		SET(b, c, d, a,  3, 22,  T4);
		SET(a, b, c, d,  4,  7,  T5);
		SET(d, a, b, c,  5, 12,  T6);
		SET(c, d, a, b,  6, 17,  T7);
		SET(b, c, d, a,  7, 22,  T8);
		SET(a, b, c, d,  8,  7,  T9);
		SET(d, a, b, c,  9, 12, T10);
		SET(c, d, a, b, 10, 17, T11);
		SET(b, c, d, a, 11, 22, T12);
		SET(a, b, c, d, 12,  7, T13);
		SET(d, a, b, c, 13, 12, T14);
		SET(c, d, a, b, 14, 17, T15);
		SET(b, c, d, a, 15, 22, T16);
#undef SET

		/* Round 2. */
		/* Let [abcd k s i] denote the operation
			 a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + G(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  1,  5, T17);
		SET(d, a, b, c,  6,  9, T18);
		SET(c, d, a, b, 11, 14, T19);
		SET(b, c, d, a,  0, 20, T20);
		SET(a, b, c, d,  5,  5, T21);
		SET(d, a, b, c, 10,  9, T22);
		SET(c, d, a, b, 15, 14, T23);
		SET(b, c, d, a,  4, 20, T24);
		SET(a, b, c, d,  9,  5, T25);
		SET(d, a, b, c, 14,  9, T26);
		SET(c, d, a, b,  3, 14, T27);
		SET(b, c, d, a,  8, 20, T28);
		SET(a, b, c, d, 13,  5, T29);
		SET(d, a, b, c,  2,  9, T30);
		SET(c, d, a, b,  7, 14, T31);
		SET(b, c, d, a, 12, 20, T32);
#undef SET

		/* Round 3. */
		/* Let [abcd k s t] denote the operation
			 a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + H(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  5,  4, T33);
		SET(d, a, b, c,  8, 11, T34);
		SET(c, d, a, b, 11, 16, T35);
		SET(b, c, d, a, 14, 23, T36);
		SET(a, b, c, d,  1,  4, T37);
		SET(d, a, b, c,  4, 11, T38);
		SET(c, d, a, b,  7, 16, T39);
		SET(b, c, d, a, 10, 23, T40);
		SET(a, b, c, d, 13,  4, T41);
		SET(d, a, b, c,  0, 11, T42);
		SET(c, d, a, b,  3, 16, T43);
		SET(b, c, d, a,  6, 23, T44);
		SET(a, b, c, d,  9,  4, T45);
		SET(d, a, b, c, 12, 11, T46);
		SET(c, d, a, b, 15, 16, T47);
		SET(b, c, d, a,  2, 23, T48);
#undef SET

		/* Round 4. */
		/* Let [abcd k s t] denote the operation
			 a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + I(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  0,  6, T49);
		SET(d, a, b, c,  7, 10, T50);
		SET(c, d, a, b, 14, 15, T51);
		SET(b, c, d, a,  5, 21, T52);
		SET(a, b, c, d, 12,  6, T53);
		SET(d, a, b, c,  3, 10, T54);
		SET(c, d, a, b, 10, 15, T55);
		SET(b, c, d, a,  1, 21, T56);
		SET(a, b, c, d,  8,  6, T57);
		SET(d, a, b, c, 15, 10, T58);
		SET(c, d, a, b,  6, 15, T59);
		SET(b, c, d, a, 13, 21, T60);
		SET(a, b, c, d,  4,  6, T61);
		SET(d, a, b, c, 11, 10, T62);
		SET(c, d, a, b,  2, 15, T63);
		SET(b, c, d, a,  9, 21, T64);
#undef SET

		/* Then perform the following additions. (That is increment each
		   of the four registers by the value it had before this block
		   was started.) */
		pms->state[0] += a;
		pms->state[1] += b;
		pms->state[2] += c;
		pms->state[3] += d;
	}

	void MD5_Init(MD5_CTX *pms)
	{
		pms->count[0] = pms->count[1] = 0;
		pms->state[0] = 0x67452301;
		pms->state[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
		pms->state[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
		pms->state[3] = 0x10325476;
	}

	void MD5_Update(MD5_CTX *pms, const uint8_t *data, size_t nbytes)
	{
		const uint8_t *p = data;
		size_t left = nbytes;
		size_t offset = (pms->count[0] >> 3) & 63;
		uint32_t nbits = (uint32_t)(nbytes << 3);

		if (nbytes <= 0)
			return;

		/* Update the message length. */
		pms->count[1] += nbytes >> 29;
		pms->count[0] += nbits;
		if (pms->count[0] < nbits)
			pms->count[1]++;

		/* Process an initial partial block. */
		if (offset)
		{
			size_t copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

			std::memcpy(pms->buffer + offset, p, copy);
			if (offset + copy < 64)
				return;
			p += copy;
			left -= copy;
			md5_process(pms, pms->buffer);
		}

		/* Process full blocks. */
		for (; left >= 64; p += 64, left -= 64)
			md5_process(pms, p);

		/* Process a final partial block. */
		if (left)
			std::memcpy(pms->buffer, p, left);
	}

	void MD5_Finish(MD5_CTX *pms, uint8_t *digest)
	{
		static const uint8_t pad[64] = {
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		uint8_t data[8];
		size_t i;

		/* Save the length before padding. */
		for (i = 0; i < 8; ++i)
			data[i] = (uint8_t)(pms->count[i >> 2] >> ((i & 3) << 3));
		/* Pad to 56 bytes mod 64. */
		MD5_Update(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
		/* Append the length. */
		MD5_Update(pms, data, 8);
		for (i = 0; i < 16; ++i)
			digest[i] = (uint8_t)(pms->state[i >> 2] >> ((i & 3) << 3));
	}
}

namespace wuya
{
	md5::md5():context_(0)
	{
		context_ = new ::MD5_CTX;
	}
	md5::md5(const char* str):context_(0)
	{
		context_ = new MD5_CTX;
		init(str);
	}

	md5::~md5()
	{
		delete context_;
	}

	void md5::init(const char* str)
	{
		source_ = str;
		MD5_Init(context_);
	}
	void md5::do_digest()
	{
		MD5_Update(context_, (::uint8_t*)source_, strlen(source_));
		MD5_Finish(context_, (::uint8_t*)result_);
		result_[16] = '\0';
		for (int i = 0; i < 16; ++i)
			std::sprintf(hex_result_ + i * 2, "%02x", result_[i]);
		hex_result_[16*2+1] = '\0';
	}

	const unsigned char* md5::get_result()
	{
		return result_;
	}

	const char* md5::get_sorce()
	{
		return source_;
	}

	char* md5::get_hex_result()
	{
		return hex_result_;
	}
}


#endif


