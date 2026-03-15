/* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/* This file is originally from the mysql distribution. Coded by monty */

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

#ifndef NOT_FIXED_DEC
#define NOT_FIXED_DEC			31
#endif

#include <cstring>

#define STRING_WITH_LEN(X)  ((const char*) X), ((uint) (sizeof(X) - 1))

namespace mysql_parser
{

class String;
auto sortcmp(const String *a,const String *b, CHARSET_INFO *cs) -> int;
auto copy_if_not_alloced(String *a,String *b,uint32 arg_length) -> String *;
auto copy_and_convert(char *to, uint32 to_length, CHARSET_INFO *to_cs,
			const char *from, uint32 from_length,
			CHARSET_INFO *from_cs, uint *errors) -> uint32;

class String
{
  char *Ptr;
  uint32 str_length,Alloced_length;
  bool alloced;
  CHARSET_INFO *str_charset;
public:
  String()
  { 
    Ptr=0; str_length=Alloced_length=0; alloced=0; 
    str_charset= &my_charset_bin; 
  }
  String(uint32 length_arg)
  { 
    alloced=0; Alloced_length=0; (void) real_alloc(length_arg); 
    str_charset= &my_charset_bin;
  }
  String(const char *str, CHARSET_INFO *cs)
  { 
    Ptr=(char*) str; str_length=(uint) strlen(str); Alloced_length=0; alloced=0;
    str_charset=cs;
  }
  String(const char *str,uint32 len, CHARSET_INFO *cs)
  { 
    Ptr=(char*) str; str_length=len; Alloced_length=0; alloced=0;
    str_charset=cs;
  }
  String(char *str,uint32 len, CHARSET_INFO *cs)
  { 
    Ptr=(char*) str; Alloced_length=str_length=len; alloced=0;
    str_charset=cs;
  }
  String(const String &str)
  { 
    Ptr=str.Ptr ; str_length=str.str_length ;
    Alloced_length=str.Alloced_length; alloced=0; 
    str_charset=str.str_charset;
  }
  static void *operator new(size_t size/*, MEM_ROOT *mem_root*/)
  { return (void*) alloc_root((uint) size); }
  static void operator delete(void *ptr_arg,size_t size)
  { TRASH(ptr_arg, size); }
  static void operator delete(void *ptr_arg/*, MEM_ROOT *mem_root*/)
  { /* never called */ }
  ~String() { free(); }

  inline auto set_charset(CHARSET_INFO *charset) -> void { str_charset= charset; }
  inline auto charset() const -> CHARSET_INFO * { return str_charset; }
  inline auto length() const -> uint32 { return str_length;}
  inline auto alloced_length() const -> uint32 { return Alloced_length;}
  inline char& operator [] (uint32 i) const { return Ptr[i]; }
  inline auto length(uint32 len) -> void { str_length=len ; }
  inline auto is_empty() -> bool { return (str_length == 0); }
  inline auto mark_as_const() -> void { Alloced_length= 0;}
  inline auto ptr() const -> const char * { return Ptr; }
  inline auto c_ptr() -> char * {
    if (!Ptr || Ptr[str_length])		/* Should be safe */
      (void) realloc(str_length);
    return Ptr;
  }
  inline auto c_ptr_quick() -> char * {
    if (Ptr && str_length < Alloced_length)
      Ptr[str_length]=0;
    return Ptr;
  }
  inline auto c_ptr_safe() -> char * {
    if (Ptr && str_length < Alloced_length)
      Ptr[str_length]=0;
    else
      (void) realloc(str_length);
    return Ptr;
  }

  auto set(String &str,uint32 offset,uint32 arg_length) -> void {
    DBUG_ASSERT(&str != this);
    free();
    Ptr=(char*) str.ptr()+offset; str_length=arg_length; alloced=0;
    if (str.Alloced_length)
      Alloced_length=str.Alloced_length-offset;
    else
      Alloced_length=0;
    str_charset=str.str_charset;
  }
  inline auto set(const char *str,uint32 arg_length, CHARSET_INFO *cs) -> void {
    free();
    Ptr=(char*) str; str_length=arg_length; Alloced_length=0 ; alloced=0;
    str_charset=cs;
  }
  auto set_ascii(const char *str, uint32 arg_length) -> bool;
  inline auto set_quick(char *str,uint32 arg_length, CHARSET_INFO *cs) -> void {
    if (!alloced)
    {
      Ptr=(char*) str; str_length=Alloced_length=arg_length;
    }
    str_charset=cs;
  }
  auto set(longlong num, CHARSET_INFO *cs) -> bool;
  auto set(ulonglong num, CHARSET_INFO *cs) -> bool;
  auto set(double num,uint decimals, CHARSET_INFO *cs) -> bool;

  /*
    PMG 2004.11.12
    This is a method that works the same as perl's "chop". It simply
    drops the last character of a string. This is useful in the case
    of the federated storage handler where I'm building a unknown
    number, list of values and fields to be used in a sql insert
    statement to be run on the remote server, and have a comma after each.
    When the list is complete, I "chop" off the trailing comma

    ex. 
      String stringobj; 
      stringobj.append("VALUES ('foo', 'fi', 'fo',");
      stringobj.chop();
      stringobj.append(")");

    In this case, the value of string was:

    VALUES ('foo', 'fi', 'fo',
    VALUES ('foo', 'fi', 'fo'
    VALUES ('foo', 'fi', 'fo')
      
  */
  inline auto chop() -> void {
    Ptr[str_length--]= '\0'; 
  }

  inline auto free() -> void {
    if (alloced)
    {
      alloced=0;
      Alloced_length=0;
      my_free(Ptr,MYF(0));
      Ptr=0;
      str_length=0;				/* Safety */
    }
  }
  inline auto alloc(uint32 arg_length) -> bool {
    if (arg_length < Alloced_length)
      return 0;
    return real_alloc(arg_length);
  }
  auto real_alloc(uint32 arg_length) -> bool;			// Empties old string
  auto realloc(uint32 arg_length) -> bool;
  inline void shrink(uint32 arg_length)		// Shrink buffer
  {
    if (arg_length < Alloced_length)
    {
      char *new_ptr;
      if (!(new_ptr=(char*) my_realloc(Ptr,arg_length,MYF(0))))
      {
	Alloced_length = 0;
	real_alloc(arg_length);
      }
      else
      {
	Ptr=new_ptr;
	Alloced_length=arg_length;
      }
    }
  }
  auto is_alloced() -> bool { return alloced; }
  inline String& operator = (const String &s)
  {
    if (&s != this)
    {
      /*
        It is forbidden to do assignments like 
        some_string = substring_of_that_string
       */
      DBUG_ASSERT(!s.uses_buffer_owned_by(this));
      free();
      Ptr=s.Ptr ; str_length=s.str_length ; Alloced_length=s.Alloced_length;
      alloced=0;
    }
    return *this;
  }

  auto copy() -> bool;					// Alloc string if not alloced
  auto copy(const String &s) -> bool;			// Allocate new string
  auto copy(const char *s,uint32 arg_length, CHARSET_INFO *cs) -> bool;	// Allocate new string
  static auto needs_conversion(uint32 arg_length,
  			       CHARSET_INFO *cs_from, CHARSET_INFO *cs_to,
			       uint32 *offset) -> bool;
  auto copy_aligned(const char *s, uint32 arg_length, uint32 offset,
		    CHARSET_INFO *cs) -> bool;
  auto set_or_copy_aligned(const char *s, uint32 arg_length, CHARSET_INFO *cs) -> bool;
  auto copy(const char*s,uint32 arg_length, CHARSET_INFO *csfrom,
	    CHARSET_INFO *csto, uint *errors) -> bool;
  auto append(const String &s) -> bool;
  auto append(const char *s) -> bool;
  auto append(const char *s,uint32 arg_length) -> bool;
  auto append(const char *s,uint32 arg_length, CHARSET_INFO *cs) -> bool;
  auto append(IO_CACHE* file, uint32 arg_length) -> bool;
  auto append_with_prefill(const char *s, uint32 arg_length, 
			   uint32 full_length, char fill_char) -> bool;
  auto strstr(const String &search,uint32 offset=0) -> int; // Returns offset to substring or -1
  auto strrstr(const String &search,uint32 offset=0) -> int; // Returns offset to substring or -1
  auto replace(uint32 offset,uint32 arg_length,const char *to,uint32 length) -> bool;
  auto replace(uint32 offset,uint32 arg_length,const String &to) -> bool;
  inline auto append(char chr) -> bool {
    if (str_length < Alloced_length)
    {
      Ptr[str_length++]=chr;
    }
    else
    {
      if (realloc(str_length+1))
	return 1;
      Ptr[str_length++]=chr;
    }
    return 0;
  }
  auto fill(uint32 max_length,char fill) -> bool;
  auto strip_sp() -> void;
  friend auto sortcmp(const String *a,const String *b, CHARSET_INFO *cs) -> int;
  friend auto stringcmp(const String *a,const String *b) -> int;
  friend auto copy_if_not_alloced(String *a,String *b,uint32 arg_length) -> String *;
  auto numchars() -> uint32;
  auto charpos(int i,uint32 offset=0) -> int;

  auto reserve(uint32 space_needed) -> int {
    return realloc(str_length + space_needed);
  }
  auto reserve(uint32 space_needed, uint32 grow_by) -> int;

  /*
    The following append operations do NOT check alloced memory
    q_*** methods writes values of parameters itself
    qs_*** methods writes string representation of value
  */
  auto q_append(const char c) -> void {
    Ptr[str_length++] = c;
  }
  auto q_append(const uint32 n) -> void {
    int4store(Ptr + str_length, n);
    str_length += 4;
  }
  auto q_append(double d) -> void {
    float8store(Ptr + str_length, d);
    str_length += 8;
  }
  auto q_append(double *d) -> void {
    float8store(Ptr + str_length, *d);
    str_length += 8;
  }
  auto q_append(const char *data, uint32 data_len) -> void {
    memcpy(Ptr + str_length, data, data_len);
    str_length += data_len;
  }

  auto write_at_position(int position, uint32 value) -> void {
    int4store(Ptr + position,value);
  }

  auto qs_append(const char *str, uint32 len) -> void;
  auto qs_append(double d) -> void;
  auto qs_append(double *d) -> void;
  inline auto qs_append(const char c) -> void {
     Ptr[str_length]= c;
     str_length++;
  }
  auto qs_append(int i) -> void;
  auto qs_append(uint i) -> void;

  /* Inline (general) functions used by the protocol functions */

  inline auto prep_append(uint32 arg_length, uint32 step_alloc) -> char * {
    uint32 new_length= arg_length + str_length;
    if (new_length > Alloced_length)
    {
      if (realloc(new_length + step_alloc))
        return 0;
    }
    uint32 old_length= str_length;
    str_length+= arg_length;
    return Ptr+ old_length;			/* Area to use */
  }

  inline auto append(const char *s, uint32 arg_length, uint32 step_alloc) -> bool {
    uint32 new_length= arg_length + str_length;
    if (new_length > Alloced_length && realloc(new_length + step_alloc))
      return TRUE;
    memcpy(Ptr+str_length, s, arg_length);
    str_length+= arg_length;
    return FALSE;
  }
  auto print(String *print) -> void;

  /* Swap two string objects. Efficient way to exchange data without memcpy. */
  auto swap(String &s) -> void;

  inline auto uses_buffer_owned_by(const String *s) const -> bool {
    return (s->alloced && Ptr >= s->Ptr && Ptr < s->Ptr + s->str_length);
  }
};

} // namespace mysql_parser
