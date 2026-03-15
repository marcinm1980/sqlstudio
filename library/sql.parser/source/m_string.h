/*
* Copyright (c) 2000, 2016 Oracle and/or its affiliates. All rights reserved.
* Copyright (c) 2026 dev4fun. All rights reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; version 2 of the
* License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301  USA
*/

/* There may be prolems include all of theese. Try to test in
   configure with ones are needed? */

/*  This is needed for the definitions of strchr... on solaris */

#ifndef _m_string_h
#define _m_string_h
#ifndef __USE_GNU
#define __USE_GNU				/* We want to use stpcpy */
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

/* need by my_vsnprintf */
#include <stdarg.h> 

/* Correct some things for UNIXWARE7 */
#ifdef HAVE_UNIXWARE7_THREADS
#undef HAVE_STRINGS_H
#undef HAVE_MEMORY_H
#define HAVE_MEMCPY
#ifndef HAVE_MEMMOVE
#define HAVE_MEMMOVE
#endif
#undef HAVE_BCMP
#undef bcopy
#undef bcmp
#undef bzero
#endif /* HAVE_UNIXWARE7_THREADS */
#ifdef _AIX
#undef HAVE_BCMP
#endif

/*  This is needed for the definitions of bzero... on solaris */
#if defined(HAVE_STRINGS_H) && !defined(HAVE_mit_thread)
#include <strings.h>
#endif

/*  This is needed for the definitions of memcpy... */
#if defined(HAVE_MEMMOVE) || defined(HAVE_MEMCPY)
#include <memory.h>
#endif

#if !defined(HAVE_MEMCPY) && !defined(HAVE_MEMMOVE)
# define memcpy(d, s, n)	bcopy ((s), (d), (n))
# define memset(A,C,B)		bfill((A),(B),(C))
# define memmove(d, s, n)	bmove ((d), (s), (n))
#elif defined(HAVE_MEMMOVE)
# define bmove(d, s, n)		memmove((d), (s), (n))
#else
# define memmove(d, s, n)	bmove((d), (s), (n)) /* our bmove */
#endif

/* Unixware 7 */
#if !defined(HAVE_BFILL)
# define bfill(A,B,C)           memset((A),(C),(B))
# define bmove_align(A,B,C)    memcpy((A),(B),(C))
#endif

#if !defined(HAVE_BCMP)
# define bcopy(s, d, n)		memcpy((d), (s), (n))
# define bcmp(A,B,C)		memcmp((A),(B),(C))
# define bzero(A,B)		memset((A),0,(B))
# define bmove_align(A,B,C)    memcpy((A),(B),(C))
#endif

namespace mysql_parser
{

//#if defined(__cplusplus) && !defined(OS2)
//extern "C" {
//#endif

/*
  my_str_malloc() and my_str_free() are assigned to implementations in
  strings/alloc.c, but can be overridden in the calling program.
 */
extern void *(*my_str_malloc)(size_t);
extern void (*my_str_free)(void *);

#if defined(HAVE_STPCPY) && !defined(HAVE_mit_thread)
#define strmov(A,B) stpcpy((A),(B))
#ifndef stpcpy
extern auto stpcpy(char *, const char *) -> char *;	/* For AIX with gcc 2.95.3 */
#endif
#endif

/* Declared in int2str() */
extern char NEAR _dig_vec_upper[];
extern char NEAR _dig_vec_lower[];

#ifdef BAD_STRING_COMPILER
#define strmov(A,B)  (memccpy(A,B,0,INT_MAX)-1)
#else
#define strmov_overlapp(A,B) strmov(A,B)
#define strmake_overlapp(A,B,C) strmake(A,B,C)
#endif

#ifdef BAD_MEMCPY			/* Problem with gcc on Alpha */
#define memcpy_fixed(A,B,C) bmove((A),(B),(C))
#else
#define memcpy_fixed(A,B,C) memcpy((A),(B),(C))
#endif

#ifdef MSDOS
#undef bmove_align
#define bmove512(A,B,C) bmove_align(A,B,C)
extern	auto bmove_align(gptr dst,const gptr src,uint len) -> void;
#endif

#if (!defined(USE_BMOVE512) || defined(HAVE_purify)) && !defined(bmove512)
#define bmove512(A,B,C) memcpy(A,B,C)
#endif

	/* Prototypes for string functions */

#if !defined(bfill) && !defined(HAVE_BFILL)
extern	auto bfill(gptr dst,uint len,pchar fill) -> void;
#endif

#if !defined(bzero) && !defined(HAVE_BZERO)
extern	auto bzero(gptr dst,uint len) -> void;
#endif

#if !defined(bcmp) && !defined(HAVE_BCMP)
extern	auto bcmp(const char *s1,const char *s2,uint len) -> int;
#endif
#ifdef HAVE_purify
extern	auto my_bcmp(const char *s1,const char *s2,uint len) -> int;
#undef bcmp
#define bcmp(A,B,C) my_bcmp((A),(B),(C))
#endif

#ifndef bmove512
extern	auto bmove512(gptr dst,const gptr src,uint len) -> void;
#endif

#if !defined(HAVE_BMOVE) && !defined(bmove)
extern	auto bmove(char *dst, const char *src,uint len) -> void;
#endif

extern	auto bmove_upp(char *dst,const char *src,uint len) -> void;
extern	auto bchange(char *dst,uint old_len,const char *src,
		     uint new_len,uint tot_len) -> void;
extern	auto strappend(char *s,uint len,pchar fill) -> void;
//unused extern	char *strend(const char *s);
extern  auto strcend(const char *, pchar) -> char *;
extern	auto strfield(char *src,int fields,int chars,int blanks,
			   int tabch) -> char *;
extern	auto strfill(my_string s,uint len,pchar fill) -> char *;
extern	auto strinstr(const char *str,const char *search) -> uint;
extern  auto r_strinstr(reg1 my_string str,int from, reg4 my_string search) -> uint;
extern	auto strkey(char *dst,char *head,char *tail,char *flags) -> char *;
extern	auto strmake(char *dst,const char *src,uint length) -> char *;
#ifndef strmake_overlapp
extern	auto strmake_overlapp(char *dst,const char *src, uint length) -> char *;
#endif

#ifndef strmov
extern	auto strmov(char *dst,const char *src) -> char *;
#endif
extern	auto strnmov(char *dst,const char *src,uint n) -> char *;
extern	auto strsuff(const char *src,const char *suffix) -> char *;
extern	auto strcont(const char *src,const char *set) -> char *;
extern	char *strxcat _VARARGS((char *dst,const char *src, ...));
extern	char *strxmov _VARARGS((char *dst,const char *src, ...));
extern	char *strxcpy _VARARGS((char *dst,const char *src, ...));
extern	char *strxncat _VARARGS((char *dst,uint len, const char *src, ...));
extern	char *strxnmov _VARARGS((char *dst,uint len, const char *src, ...));
extern	char *strxncpy _VARARGS((char *dst,uint len, const char *src, ...));

/* Prototypes of normal stringfunctions (with may ours) */

#ifdef WANT_STRING_PROTOTYPES
extern auto strcat(char *, const char *) -> char *;
extern auto strchr(const char *, pchar) -> char *;
extern auto strrchr(const char *, pchar) -> char *;
extern auto strcpy(char *, const char *) -> char *;
extern auto strcmp(const char *, const char *) -> int;
#ifndef __GNUC__
extern auto strlen(const char *) -> size_t;
#endif
#endif
#ifndef HAVE_STRNLEN
extern auto strnlen(const char *s, uint n) -> uint;
#endif

#if !defined(__cplusplus)
#ifndef HAVE_STRPBRK
extern auto strpbrk(const char *, const char *) -> char *;
#endif
#ifndef HAVE_STRSTR
extern auto strstr(const char *, const char *) -> char *;
#endif
#endif
extern auto is_prefix(const char *, const char *) -> int;

/* Conversion routines */
auto my_strtod(const char *str, char **end, int *error) -> double;
auto my_atof(const char *nptr) -> double;

extern auto llstr(longlong value,char *buff) -> char *;
#ifndef HAVE_STRTOUL
extern auto strtol(const char *str, char **ptr, int base) -> long;
extern auto strtoul(const char *str, char **ptr, int base) -> ulong;
#endif

extern auto int2str(long val, char *dst, int radix, int upcase) -> char *;
extern auto int10_to_str(long val,char *dst,int radix) -> char *;
extern auto str2int(const char *src,int radix,long lower,long upper,
			 long *val) -> char *;
auto my_strtoll10(const char *nptr, char **endptr, int *error) -> longlong;
#if SIZEOF_LONG == SIZEOF_LONG_LONG
#define longlong2str(A,B,C) int2str((A),(B),(C),1)
#define longlong10_to_str(A,B,C) int10_to_str((A),(B),(C))
#undef strtoll
#define strtoll(A,B,C) strtol((A),(B),(C))
#define strtoull(A,B,C) strtoul((A),(B),(C))
#ifndef HAVE_STRTOULL
#define HAVE_STRTOULL
#endif
#ifndef HAVE_STRTOLL
#define HAVE_STRTOLL
#endif
#else
#ifdef HAVE_LONG_LONG
extern auto longlong2str(longlong val,char *dst,int radix) -> char *;
extern auto longlong10_to_str(longlong val,char *dst,int radix) -> char *;
#if (!defined(HAVE_STRTOULL) || defined(HAVE_mit_thread)) || defined(NO_STRTOLL_PROTO)
extern auto strtoll(const char *str, char **ptr, int base) -> longlong;
extern auto strtoull(const char *str, char **ptr, int base) -> ulonglong;
#endif
#endif
#endif

/* my_vsnprintf.c */

extern auto my_vsnprintf( char *str, size_t n,
                                const char *format, va_list ap ) -> int;
extern auto my_snprintf(char* to, size_t n, const char* fmt, ...) -> int;

//#if defined(__cplusplus) && !defined(OS2)
//}
//#endif

} // namespace mysql_parser

#endif
