/******************************************************************************
 *                          Reusable MSP430 printf()
 *
 * Description: This printf function was written by oPossum and originally
 *              posted on the 43oh.com forums. For more information on this
 *              code, please see the link below.
 *
 *              http://www.43oh.com/forum/viewtopic.php?f=10&t=1732
 *
 *              A big thanks to oPossum for sharing such great code!
 *
 * Author:  oPossum
 * Source:  http://www.43oh.com/forum/viewtopic.php?f=10&t=1732
 * Date:    10-17-11
 *
 * Note: This comment section was written by Nicholas J. Conn on 06-07-2012
 *       for use on NJC's MSP430 LaunchPad Blog.
 ******************************************************************************/
#include <stdarg.h>
void putc(unsigned);
void puts(char *);

static const unsigned long long dv[] = {

		100000000000000, // +0
		10000000000000, // +1
		1000000000000, // +2
		100000000000, // +3
		10000000000, // +4
//  4294967296      // 32 bit unsigned max
		1000000000,// +5
		100000000, // +6
		10000000, // +7
		1000000, // +8
		100000, // +9
//       65535      // 16 bit unsigned max
		10000, // +10
		1000, // +11
		100, // +12
		10, // +13
		1, // +14
		};

static void xtoa(unsigned long long x, const unsigned long long*dp) {
	char c;
	unsigned long long d;
	if (x) {
		while (x < *dp)
			++dp;
		do {
			d = *dp++;
			c = '0';
			while (x >= d)
				++c, x -= d;
			putc(c);
		} while (!(d & 1));
	} else
		putc('0');
}

static void puth(unsigned n) {
	static const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8',
			'9', 'A', 'B', 'C', 'D', 'E', 'F' };
	putc(hex[n & 15]);
}

void printf(char *format, ...)
{
	char c;
	int i;
	long n;
	long long r;

	va_list a;
	va_start(a, format);
	while((c = *format++)) {
		if(c == '%') {
			switch(c = *format++) {
				case 's': // String
					puts(va_arg(a, char*));
					break;
				case 'c':// Char
					putc((char)va_arg(a, int));
				break;
				case 'i':// 16 bit Integer
				case 'u':// 16 bit Unsigned
					i = va_arg(a, int);
					if(c == 'i' && i < 0) i = -i, putc('-');
					xtoa((unsigned)i, dv + 10);
				break;
				case 'l':// 32 bit Long
				case 'n':// 32 bit uNsigned loNg
					n = va_arg(a, long);
					if(c == 'l' && n < 0) n = -n, putc('-');
					xtoa((unsigned long)n, dv+5);
				break;
				case 'y':// 64 bit signed long long
				case 'r':// 64 bit unsigned long long
					r = va_arg(a, long long);
					if(c == 'y' && n < 0) r = -r, putc('-');
					xtoa((unsigned long long)r, dv);
					break;
				case 'x':// 16 bit heXadecimal
					i = va_arg(a, int);
					puth(i >> 12);
					puth(i >> 8);
					puth(i >> 4);
					puth(i);
				break;
				case 0: return;
				default: goto bad_fmt;
			}
		} else
			bad_fmt: putc(c);
	}
	va_end(a);
}
