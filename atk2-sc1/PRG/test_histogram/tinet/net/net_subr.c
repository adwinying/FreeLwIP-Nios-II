/*
 *  ネットワーク汎用サポートルーチン
 */

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include <net/net.h>

/*
 *  変数
 */

static uint32_t next = ULONG_C(1);

/*
 *  net_rand -- 乱数を返す。
 */

uint32_t
net_rand (void)
{
	next = (next * 1103515245 + 12345) % (ULONG_C(0x7fffffff) + 1);
	return next;
	}

/*
 *  srand -- 乱数を初期化する。
 */

void
net_srand (uint32_t seed)
{
	next += seed;
	}

/*
 *  convert_hexdigit -- 16進数→文字列変換
 */

int_t
convert_hexdigit (char *buf, uint_t val, int_t radix, int_t width, char padchar)
{
	static const char radhex[] = "0123456789abcdef";

	char	digits[11], *start;
	int_t	ix, pad;

	ix = 0;
	start = buf;
	do {
		digits[ix ++] = radhex[(val % radix) & 0x0f];
		val /= radix;
		} while (val != 0);
	for (pad = ix; pad < width; pad ++)
		*buf ++ = padchar;
	while (ix -- > 0)
		*buf ++ = digits[ix];
	*buf = '\0';
	return (int_t)(buf - start);
	}

#ifdef SUPPORT_ETHER

/*
 *  mac2str -- MAC アドレスを文字列に変換する。
 */

char *
mac2str (char *buf, uint8_t *macaddr)
{
	static char	addr_sbuf[NUM_MACADDR_STR_BUFF][sizeof("00:00:00:00:00:00")];
	static int_t	bix = NUM_MACADDR_STR_BUFF;

	char *start;

	if (buf == NULL) {
		syscall(GetResource(RES_MAC2STR_BUFF_LOCK));
		buf = addr_sbuf[-- bix];
		if (bix <= 0)
			bix = NUM_MACADDR_STR_BUFF;
		syscall(ReleaseResource(RES_MAC2STR_BUFF_LOCK));
		}

	start = buf;
	buf += convert_hexdigit(buf, (uint_t)(*macaddr ++), 16, 2, '0');
	*(buf ++) = ':';
	buf += convert_hexdigit(buf, (uint_t)(*macaddr ++), 16, 2, '0');
	*(buf ++) = ':';
	buf += convert_hexdigit(buf, (uint_t)(*macaddr ++), 16, 2, '0');
	*(buf ++) = ':';
	buf += convert_hexdigit(buf, (uint_t)(*macaddr ++), 16, 2, '0');
	*(buf ++) = ':';
	buf += convert_hexdigit(buf, (uint_t)(*macaddr ++), 16, 2, '0');
	*(buf ++) = ':';
	buf += convert_hexdigit(buf, (uint_t)(*macaddr ++), 16, 2, '0');
	*buf = '\0';
	return start;
	}

#endif	/* of #ifdef SUPPORT_ETHER */

#if _NET_CFG_BYTE_ORDER == _NET_CFG_LITTLE_ENDIAN

/*
 *  rev_memcpy_word -- 反転メモリコピー（4 バイト）
 *
 *    バイト単位にアクセスする事により、
 *    境界へのアラインの問題を解決する。
 */

void
rev_memcpy_word (uint8_t *dst, uint8_t *src)
{
	*(dst    ) = *(src + 3);
	*(dst + 1) = *(src + 2);
	*(dst + 2) = *(src + 1);
	*(dst + 3) = *(src    );
	}

/*
 *  rev_memcmp_word -- 反転メモリ比較（4 バイト）
 *
 *    バイト単位にアクセスする事により、
 *    境界へのアラインの問題を解決する。
 */

int_t
rev_memcmp_word (uint8_t *data1, uint8_t *data2)
{
	int_t	ix, diff;

	for (ix = 4; ix -- > 0; ) {
		diff = *(data1 + ix) -  *(data2 + (3 - ix));
		if (diff != 0)
			return diff;
		}
	return 0;
	}

#endif	/* of #if _NET_CFG_BYTE_ORDER == _NET_CFG_LITTLE_ENDIAN */
