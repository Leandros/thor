#include <libc/bits/internal.h>
#include <libc/errno.h>
#include <libc/ctype.h>
#include <libc/limits.h>
#include <libc/string.h>
#include <libc/stdarg.h>
#include <libc/wchar.h>
#include <libc/inttypes.h>
#include <libc/math.h>
#include <libc/float.h>

DISABLE_WARNING(pragmas,pragmas,4554);
DISABLE_WARNING(pragmas,pragmas,4242);
DISABLE_WARNING(pragmas,pragmas,4244);
DISABLE_WARNING(pragmas,pragmas,4018);
DISABLE_WARNING(pragmas,pragmas,4146);

/* Some useful macros */
#define F_ERR 32
#define NL_ARGMAX 9

#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#endif

/* Convenient bit representation for modifier flags, which all fall
 * within 31 codepoints of the space character. */

#define ALT_FORM   (1U<<('#'-' '))
#define ZERO_PAD   (1U<<('0'-' '))
#define LEFT_ADJ   (1U<<('-'-' '))
#define PAD_POS    (1U<<(' '-' '))
#define MARK_POS   (1U<<('+'-' '))
#define GROUPED    (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|LEFT_ADJ|PAD_POS|MARK_POS|GROUPED)

#if UINT_MAX == ULONG_MAX
#define LONG_IS_INT
#endif

#if !defined(_WIN64)
#define ODD_TYPES
#endif

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success. */

enum {
	STATE_BARE, STATE_LPRE, STATE_LLPRE, STATE_HPRE, STATE_HHPRE, STATE_BIGLPRE,
	STATE_ZTPRE, STATE_JPRE,
	STATE_STOP,
	STATE_PTR, STATE_INT, STATE_UINT, STATE_ULLONG,
#ifndef LONG_IS_INT
	STATE_LONG, STATE_ULONG,
#else
#define STATE_LONG STATE_INT
#define STATE_ULONG STATE_UINT
#endif
	STATE_SHORT, STATE_USHORT, STATE_CHAR, STATE_UCHAR,
#ifdef ODD_TYPES
	STATE_LLONG, STATE_SIZET, STATE_IMAX, STATE_UMAX, STATE_PDIFF, STATE_UIPTR,
#else
#define STATE_LLONG STATE_ULLONG
#define STATE_SIZET STATE_ULONG
#define STATE_IMAX STATE_LLONG
#define STATE_UMAX STATE_ULLONG
#define STATE_PDIFF STATE_LONG
#define STATE_UIPTR STATE_ULONG
#endif
	STATE_DBL, STATE_LDBL,
	STATE_NOARG,
	STATE_MAXSTATE
};

#define S(x) [(x)-'A']

static const unsigned char states[]['z'-'A'+1] = {
	{ /* 0: bare types */
		S('d') = STATE_INT, S('i') = STATE_INT,
		S('o') = STATE_UINT, S('u') = STATE_UINT, S('x') = STATE_UINT, S('X') = STATE_UINT,
		S('e') = STATE_DBL, S('f') = STATE_DBL, S('g') = STATE_DBL, S('a') = STATE_DBL,
		S('E') = STATE_DBL, S('F') = STATE_DBL, S('G') = STATE_DBL, S('A') = STATE_DBL,
		S('c') = STATE_CHAR, S('C') = STATE_INT,
		S('s') = STATE_PTR, S('S') = STATE_PTR, S('p') = STATE_UIPTR, S('n') = STATE_PTR,
		S('m') = STATE_NOARG,
		S('l') = STATE_LPRE, S('h') = STATE_HPRE, S('L') = STATE_BIGLPRE,
		S('z') = STATE_ZTPRE, S('j') = STATE_JPRE, S('t') = STATE_ZTPRE,
	}, { /* 1: l-prefixed */
		S('d') = STATE_LONG, S('i') = STATE_LONG,
		S('o') = STATE_ULONG, S('u') = STATE_ULONG, S('x') = STATE_ULONG, S('X') = STATE_ULONG,
		S('e') = STATE_DBL, S('f') = STATE_DBL, S('g') = STATE_DBL, S('a') = STATE_DBL,
		S('E') = STATE_DBL, S('F') = STATE_DBL, S('G') = STATE_DBL, S('A') = STATE_DBL,
		S('c') = STATE_INT, S('s') = STATE_PTR, S('n') = STATE_PTR,
		S('l') = STATE_LLPRE,
	}, { /* 2: ll-prefixed */
		S('d') = STATE_LLONG, S('i') = STATE_LLONG,
		S('o') = STATE_ULLONG, S('u') = STATE_ULLONG,
		S('x') = STATE_ULLONG, S('X') = STATE_ULLONG,
		S('n') = STATE_PTR,
	}, { /* 3: h-prefixed */
		S('d') = STATE_SHORT, S('i') = STATE_SHORT,
		S('o') = STATE_USHORT, S('u') = STATE_USHORT,
		S('x') = STATE_USHORT, S('X') = STATE_USHORT,
		S('n') = STATE_PTR,
		S('h') = STATE_HHPRE,
	}, { /* 4: hh-prefixed */
		S('d') = STATE_CHAR, S('i') = STATE_CHAR,
		S('o') = STATE_UCHAR, S('u') = STATE_UCHAR,
		S('x') = STATE_UCHAR, S('X') = STATE_UCHAR,
		S('n') = STATE_PTR,
	}, { /* 5: L-prefixed */
		S('e') = STATE_LDBL, S('f') = STATE_LDBL, S('g') = STATE_LDBL, S('a') = STATE_LDBL,
		S('E') = STATE_LDBL, S('F') = STATE_LDBL, S('G') = STATE_LDBL, S('A') = STATE_LDBL,
		S('n') = STATE_PTR,
	}, { /* 6: z- or t-prefixed (assumed to be same size) */
		S('d') = STATE_PDIFF, S('i') = STATE_PDIFF,
		S('o') = STATE_SIZET, S('u') = STATE_SIZET,
		S('x') = STATE_SIZET, S('X') = STATE_SIZET,
		S('n') = STATE_PTR,
	}, { /* 7: j-prefixed */
		S('d') = STATE_IMAX, S('i') = STATE_IMAX,
		S('o') = STATE_UMAX, S('u') = STATE_UMAX,
		S('x') = STATE_UMAX, S('X') = STATE_UMAX,
		S('n') = STATE_PTR,
	}
};

#define OOB(x) ((unsigned)(x)-'A' > 'z'-'A')

union arg
{
	uintmax_t i;
	long double f;
	void *p;
};

static void pop_arg(union arg *arg, int type, va_list *ap)
{
	/* Give the compiler a hint for optimizing the switch. */
	if ((unsigned)type > STATE_MAXSTATE) return;
	switch (type) {
	       case STATE_PTR:	arg->p = va_arg(*ap, void *);
	break; case STATE_INT:	arg->i = va_arg(*ap, int);
	break; case STATE_UINT:	arg->i = va_arg(*ap, unsigned int);
#ifndef LONG_IS_INT
	break; case STATE_LONG:	arg->i = va_arg(*ap, long);
	break; case STATE_ULONG:	arg->i = va_arg(*ap, unsigned long);
#endif
	break; case STATE_ULLONG:	arg->i = va_arg(*ap, unsigned long long);
	break; case STATE_SHORT:	arg->i = (short)va_arg(*ap, int);
	break; case STATE_USHORT:	arg->i = (unsigned short)va_arg(*ap, int);
	break; case STATE_CHAR:	arg->i = (signed char)va_arg(*ap, int);
	break; case STATE_UCHAR:	arg->i = (unsigned char)va_arg(*ap, int);
#ifdef ODD_TYPES
	break; case STATE_LLONG:	arg->i = va_arg(*ap, long long);
	break; case STATE_SIZET:	arg->i = va_arg(*ap, size_t);
	break; case STATE_IMAX:	arg->i = va_arg(*ap, intmax_t);
	break; case STATE_UMAX:	arg->i = va_arg(*ap, uintmax_t);
	break; case STATE_PDIFF:	arg->i = va_arg(*ap, ptrdiff_t);
	break; case STATE_UIPTR:	arg->i = (uintptr_t)va_arg(*ap, void *);
#endif
	break; case STATE_DBL:	arg->f = va_arg(*ap, double);
	break; case STATE_LDBL:	arg->f = va_arg(*ap, long double);
	}
}

typedef void (*wfp)(void *, const char *, size_t);
static void __out(void *f, const char *s, size_t l)
{
	__fwrite((void*)s, l, 1, (FILE*)f);
}

static void pad(FILE *f, char c, int w, int l, int fl, wfp out)
{
	char pad[256];
	if (fl & (LEFT_ADJ | ZERO_PAD) || l >= w) return;
	l = w - l;
	memset(pad, c, l>sizeof pad ? sizeof pad : l);
	for (; l >= sizeof pad; l -= sizeof pad)
		out(f, pad, sizeof pad);
	out(f, pad, l);
}

static const char xdigits[16] = {
	"0123456789ABCDEF"
};

static char *fmt_x(uintmax_t x, char *s, int lower)
{
	for (; x; x>>=4) *--s = xdigits[(x&15)]|lower;
	return s;
}

static char *fmt_o(uintmax_t x, char *s)
{
	for (; x; x>>=3) *--s = '0' + (x&7);
	return s;
}

static char *fmt_u(uintmax_t x, char *s)
{
	unsigned long y;
	for (   ; x>ULONG_MAX; x/=10) *--s = '0' + x%10;
	for (y=x;           y; y/=10) *--s = '0' + y%10;
	return s;
}

/* Do not override this check. The floating point printing code below
 * depends on the float.h constants being right. If they are wrong, it
 * may overflow the stack. */
#if LDBL_MANT_DIG == 53
typedef char compiler_defines_long_double_incorrectly[9-(int)sizeof(long double)];
#endif

static int fmt_fp(FILE *f, long double y, int w, int p, int fl, int t, wfp out)
{
	uint32_t big[(LDBL_MANT_DIG+28)/29 + 1          // mantissa expansion
		+ (LDBL_MAX_EXP+LDBL_MANT_DIG+28+8)/9]; // exponent expansion
	uint32_t *a, *d, *r, *z;
	int e2=0, e, i, j, l;
	char buf[9+LDBL_MANT_DIG/4], *s;
	const char *prefix="-0X+0X 0X-0x+0x 0x";
	int pl;
	char ebuf0[3*sizeof(int)], *ebuf=&ebuf0[3*sizeof(int)], *estr = NULL;

	pl=1;
	if (signbit(y)) {
		y=-y;
	} else if (fl & MARK_POS) {
		prefix+=3;
	} else if (fl & PAD_POS) {
		prefix+=6;
	} else prefix++, pl=0;

	if (!isfinite(y)) {
		char *s = (t&32)?"inf":"INF";
		if (y!=y) s=(t&32)?"nan":"NAN";
		pad(f, ' ', w, 3+pl, fl&~ZERO_PAD, out);
		out(f, prefix, pl);
		out(f, s, 3);
		pad(f, ' ', w, 3+pl, fl^LEFT_ADJ, out);
		return MAX(w, 3+pl);
	}

	y = frexpl(y, &e2) * 2;
	if (y) e2--;

	if ((t|32)=='a') {
		long double round = 8.0;
		int re;

		if (t&32) prefix += 9;
		pl += 2;

		if (p<0 || p>=LDBL_MANT_DIG/4-1) re=0;
		else re=LDBL_MANT_DIG/4-1-p;

		if (re) {
			while (re--) round*=16;
			if (*prefix=='-') {
				y=-y;
				y-=round;
				y+=round;
				y=-y;
			} else {
				y+=round;
				y-=round;
			}
		}

		estr=fmt_u(e2<0 ? -e2 : e2, ebuf);
		if (estr==ebuf) *--estr='0';
		*--estr = (e2<0 ? '-' : '+');
		*--estr = t+('p'-'a');

		s=buf;
		do {
			int x=y;
			*s++=xdigits[x]|(t&32);
			y=16*(y-x);
			if (s-buf==1 && (y||p>0||(fl&ALT_FORM))) *s++='.';
		} while (y);

		if (p && s-buf-2 < p)
			l = (p+2) + (ebuf-estr);
		else
			l = (s-buf) + (ebuf-estr);

		pad(f, ' ', w, pl+l, fl, out);
		out(f, prefix, pl);
		pad(f, '0', w, pl+l, fl^ZERO_PAD, out);
		out(f, buf, s-buf);
		pad(f, '0', l-(ebuf-estr)-(s-buf), 0, 0, out);
		out(f, estr, ebuf-estr);
		pad(f, ' ', w, pl+l, fl^LEFT_ADJ, out);
		return MAX(w, pl+l);
	}
	if (p<0) p=6;

	if (y) y *= 268435456.0, e2-=28;

	if (e2<0) a=r=z=big;
	else a=r=z=big+sizeof(big)/sizeof(*big) - LDBL_MANT_DIG - 1;

	do {
		*z = y;
		y = 1000000000*(y-*z++);
	} while (y);

	while (e2>0) {
		uint32_t carry=0;
		int sh=MIN(29,e2);
		for (d=z-1; d>=a; d--) {
			uint64_t x = ((uint64_t)*d<<sh)+carry;
			*d = x % 1000000000;
			carry = x / 1000000000;
		}
		if (carry) *--a = carry;
		while (z>a && !z[-1]) z--;
		e2-=sh;
	}
	while (e2<0) {
		uint32_t carry=0, *b;
		int sh=MIN(9,-e2), need=1+(p+LDBL_MANT_DIG/3+8)/9;
		for (d=a; d<z; d++) {
			uint32_t rm = *d & (1<<sh)-1;
			*d = (*d>>sh) + carry;
			carry = (1000000000>>sh) * rm;
		}
		if (!*a) a++;
		if (carry) *z++ = carry;
		/* Avoid (slow!) computation past requested precision */
		b = (t|32)=='f' ? r : a;
		if (z-b > need) z = b+need;
		e2+=sh;
	}

	if (a<z) for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
	else e=0;

	/* Perform rounding: j is precision after the radix (possibly neg) */
	j = p - ((t|32)!='f')*e - ((t|32)=='g' && p);
	if (j < 9*(z-r-1)) {
		uint32_t x;
		/* We avoid C's broken division of negative numbers */
		d = r + 1 + ((j+9*LDBL_MAX_EXP)/9 - LDBL_MAX_EXP);
		j += 9*LDBL_MAX_EXP;
		j %= 9;
		for (i=10, j++; j<9; i*=10, j++);
		x = *d % i;
		/* Are there any significant digits past j? */
		if (x || d+1!=z) {
			long double round = 2/LDBL_EPSILON;
			long double small;
			if (*d/i & 1) round += 2;
			if (x<i/2) small=0.5;
			else if (x==i/2 && d+1==z) small=1.0;
			else small=1.5;
			if (pl && *prefix=='-') round*=-1, small*=-1;
			*d -= x;
			/* Decide whether to round by probing round+small */
			if (round+small != round) {
				*d = *d + i;
				while (*d > 999999999) {
					*d--=0;
					if (d<a) *--a=0;
					(*d)++;
				}
				for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
			}
		}
		if (z>d+1) z=d+1;
	}
	for (; z>a && !z[-1]; z--);

	if ((t|32)=='g') {
		if (!p) p++;
		if (p>e && e>=-4) {
			t--;
			p-=e+1;
		} else {
			t-=2;
			p--;
		}
		if (!(fl&ALT_FORM)) {
			/* Count trailing zeros in last place */
			if (z>a && z[-1]) for (i=10, j=0; z[-1]%i==0; i*=10, j++);
			else j=9;
			if ((t|32)=='f')
				p = MIN(p,MAX(0,9*(z-r-1)-j));
			else
				p = MIN(p,MAX(0,9*(z-r-1)+e-j));
		}
	}
	l = 1 + p + (p || (fl&ALT_FORM));
	if ((t|32)=='f') {
		if (e>0) l+=e;
	} else {
		estr=fmt_u(e<0 ? -e : e, ebuf);
		while(ebuf-estr<2) *--estr='0';
		*--estr = (e<0 ? '-' : '+');
		*--estr = t;
		l += ebuf-estr;
	}

	pad(f, ' ', w, pl+l, fl, out);
	out(f, prefix, pl);
	pad(f, '0', w, pl+l, fl^ZERO_PAD, out);

	if ((t|32)=='f') {
		if (a>r) a=r;
		for (d=a; d<=r; d++) {
			char *s = fmt_u(*d, buf+9);
			if (d!=a) while (s>buf) *--s='0';
			else if (s==buf+9) *--s='0';
			out(f, s, buf+9-s);
		}
		if (p || (fl&ALT_FORM)) out(f, ".", 1);
		for (; d<z && p>0; d++, p-=9) {
			char *s = fmt_u(*d, buf+9);
			while (s>buf) *--s='0';
			out(f, s, MIN(9,p));
		}
		pad(f, '0', p+9, 9, 0, out);
	} else {
		if (z<=a) z=a+1;
		for (d=a; d<z && p>=0; d++) {
			char *s = fmt_u(*d, buf+9);
			if (s==buf+9) *--s='0';
			if (d!=a) while (s>buf) *--s='0';
			else {
				out(f, s++, 1);
				if (p>0||(fl&ALT_FORM)) out(f, ".", 1);
			}
			out(f, s, MIN(buf+9-s, p));
			p -= buf+9-s;
		}
		pad(f, '0', p+18, 18, 0, out);
		out(f, estr, ebuf-estr);
	}

	pad(f, ' ', w, pl+l, fl^LEFT_ADJ, out);

	return MAX(w, pl+l);
}

static int getint(char **s) {
	int i;
	for (i=0; isdigit(**s); (*s)++)
		i = 10*i + (**s-'0');
	return i;
}

static int
printf_core(FILE *f, const char *fmt, va_list *ap, union arg *nl_arg, int *nl_type, wfp out)
{
	char *a, *z, *s=(char *)fmt;
	unsigned l10n=0, fl;
	int w, p;
	union arg arg = {0};
	int argpos;
	unsigned st, ps;
	int cnt=0, l=0;
	int i;
	char buf[sizeof(uintmax_t)*3+3+LDBL_MANT_DIG/4];
	const char *prefix;
	int t, pl;
	wchar_t wc[2], *ws;
	char mb[4];

	for (;;) {
		/* Update output count, end loop when fmt is exhausted */
		if (cnt >= 0) {
			if (l > INT_MAX - cnt) {
				errno = EOVERFLOW;
				cnt = -1;
			} else cnt += l;
		}
		if (!*s) break;

		/* Handle literal text and %% format specifiers */
		for (a=s; *s && *s!='%'; s++);
		for (z=s; s[0]=='%' && s[1]=='%'; z++, s+=2);
		l = z-a;
		if (f) out(f, a, l);
		if (l) continue;

		if (isdigit(s[1]) && s[2]=='$') {
			l10n=1;
			argpos = s[1]-'0';
			s+=3;
		} else {
			argpos = -1;
			s++;
		}

		/* Read modifier flags */
		for (fl=0; (unsigned)*s-' '<32 && (FLAGMASK&(1U<<(*s-' '))); s++)
			fl |= 1U<<(*s-' ');

		/* Read field width */
		if (*s=='*') {
			if (isdigit(s[1]) && s[2]=='$') {
				l10n=1;
				nl_type[s[1]-'0'] = STATE_INT;
				w = nl_arg[s[1]-'0'].i;
				s+=3;
			} else if (!l10n) {
				w = f ? va_arg(*ap, int) : 0;
				s++;
			} else return -1;
			if (w<0) fl|=LEFT_ADJ, w=-w;
		} else if ((w=getint(&s))<0) return -1;

		/* Read precision */
		if (*s=='.' && s[1]=='*') {
			if (isdigit(s[2]) && s[3]=='$') {
				nl_type[s[2]-'0'] = STATE_INT;
				p = nl_arg[s[2]-'0'].i;
				s+=4;
			} else if (!l10n) {
				p = f ? va_arg(*ap, int) : 0;
				s+=2;
			} else return -1;
		} else if (*s=='.') {
			s++;
			p = getint(&s);
		} else p = -1;

		/* Format specifier state machine */
		st=0;
		do {
			if (OOB(*s)) return -1;
			ps=st;
			st=states[st]S(*s++);
		} while (st-1<STATE_STOP);
		if (!st) return -1;

		/* Check validity of argument type (nl/normal) */
		if (st==STATE_NOARG) {
			if (argpos>=0) return -1;
		} else {
			if (argpos>=0) nl_type[argpos]=st, arg=nl_arg[argpos];
			else if (f) pop_arg(&arg, st, ap);
			else return 0;
		}

		if (!f) continue;

		z = buf + sizeof(buf);
		prefix = "-+   0X0x";
		pl = 0;
		t = s[-1];

		/* Transform ls,lc -> S,C */
		if (ps && (t&15)==3) t&=~32;

		/* - and 0 flags are mutually exclusive */
		if (fl & LEFT_ADJ) fl &= ~ZERO_PAD;

		switch(t) {
		case 'n':
			switch(ps) {
			case STATE_BARE: *(int *)arg.p = cnt; break;
			case STATE_LPRE: *(long *)arg.p = cnt; break;
			case STATE_LLPRE: *(long long *)arg.p = cnt; break;
			case STATE_HPRE: *(unsigned short *)arg.p = cnt; break;
			case STATE_HHPRE: *(unsigned char *)arg.p = cnt; break;
			case STATE_ZTPRE: *(size_t *)arg.p = cnt; break;
			case STATE_JPRE: *(uintmax_t *)arg.p = cnt; break;
			}
			continue;
		case 'p':
			p = MAX(p, 2*sizeof(void*));
			t = 'x';
			fl |= ALT_FORM;
		case 'x': case 'X':
			a = fmt_x(arg.i, z, t&32);
			if (arg.i && (fl & ALT_FORM)) prefix+=(t>>4), pl=2;
			if (0) {
		case 'o':
			a = fmt_o(arg.i, z);
			if ((fl&ALT_FORM) && p<z-a+1) p=z-a+1;
			} if (0) {
		case 'd': case 'i':
			pl=1;
			if (arg.i>INTMAX_MAX) {
				arg.i=-arg.i;
			} else if (fl & MARK_POS) {
				prefix++;
			} else if (fl & PAD_POS) {
				prefix+=2;
			} else pl=0;
		case 'u':
			a = fmt_u(arg.i, z);
			}
			if (p>=0) fl &= ~ZERO_PAD;
			if (!arg.i && !p) {
				a=z;
				break;
			}
			p = MAX(p, z-a + !arg.i);
			break;
		case 'c':
			*(a=z-(p=1))=arg.i;
			fl &= ~ZERO_PAD;
			break;
		case 'm':
		    a = strerror(errno);
		case 's':
			a = arg.p ? arg.p : "(null)";
			z = memchr(a, 0, p);
			if (!z) z=a+p;
			else p=z-a;
			fl &= ~ZERO_PAD;
			break;
		case 'C':
			wc[0] = arg.i;
			wc[1] = 0;
			arg.p = wc;
			p = -1;
		case 'S':
			ws = arg.p;
			for (i=l=0; i<0U+p && *ws && (l=wctomb(mb, *ws++))>=0 && l<=0U+p-i; i+=l);
			if (l<0) return -1;
			p = i;
			pad(f, ' ', w, p, fl, out);
			ws = arg.p;
			for (i=0; i<0U+p && *ws && i+(l=wctomb(mb, *ws++))<=p; i+=l)
				out(f, mb, l);
			pad(f, ' ', w, p, fl^LEFT_ADJ, out);
			l = w>p ? w : p;
			continue;
		case 'e': case 'f': case 'g': case 'a':
		case 'E': case 'F': case 'G': case 'A':
			l = fmt_fp(f, arg.f, w, p, fl, t, out);
			continue;
		}

		if (p < z-a) p = z-a;
		if (w < pl+p) w = pl+p;

		pad(f, ' ', w, pl+p, fl, out);
		out(f, prefix, pl);
		pad(f, '0', w, pl+p, fl^ZERO_PAD, out);
		pad(f, '0', p, z-a, 0, out);
		out(f, a, z-a);
		pad(f, ' ', w, pl+p, fl^LEFT_ADJ, out);

		l = w;
	}

	if (f) return cnt;
	if (!l10n) return 0;

	for (i=1; i<=NL_ARGMAX && nl_type[i]; i++)
		pop_arg(nl_arg+i, nl_type[i], ap);
	for (; i<=NL_ARGMAX && !nl_type[i]; i++);
	if (i<=NL_ARGMAX) return -1;
	return 1;
}

int vfprintf(FILE *__restrict__ f, const char *__restrict__ fmt, va_list ap)
{
	va_list ap2;
	int nl_type[NL_ARGMAX+1] = {0};
	union arg nl_arg[NL_ARGMAX+1];
	int ret;

	/* the copy allows passing va_list* even if va_list is an array */
	va_copy(ap2, ap);
	if (printf_core(0, fmt, &ap2, nl_arg, nl_type, &__out) < 0) {
		va_end(ap2);
		return -1;
	}

	FILE_LOCK(f);
	ret = printf_core(f, fmt, &ap2, nl_arg, nl_type, &__out);
	FILE_UNLOCK(f);
	va_end(ap2);
	return ret;
}

int
__vvfprintf(void *f, const char *__restrict__ fmt, va_list ap, wfp out)
{
	va_list ap2;
	int nl_type[NL_ARGMAX+1] = {0};
	union arg nl_arg[NL_ARGMAX+1];
	int ret;

	/* the copy allows passing va_list* even if va_list is an array */
	va_copy(ap2, ap);
	if (printf_core(0, fmt, &ap2, nl_arg, nl_type, out) < 0) {
		va_end(ap2);
		return -1;
	}

	ret = printf_core(f, fmt, &ap2, nl_arg, nl_type, out);
	va_end(ap2);
	return ret;
}

ENABLE_WARNING(pragmas,pragmas,4554);
ENABLE_WARNING(pragmas,pragmas,4242);
ENABLE_WARNING(pragmas,pragmas,4244);
ENABLE_WARNING(pragmas,pragmas,4018);
ENABLE_WARNING(pragmas,pragmas,4146);

