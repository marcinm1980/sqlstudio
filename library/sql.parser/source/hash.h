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

/* Dynamic hashing of record with different key-length */

#ifndef _hash_h
#define _hash_h

namespace mysql_parser
{

//#ifdef	__cplusplus
//extern "C" {
//#endif

/*
  Overhead to store an element in hash
  Can be used to approximate memory consumption for a hash
 */
#define HASH_OVERHEAD (sizeof(char*)*2)

typedef byte *(*hash_get_key)(const byte *,uint*,my_bool);
typedef void (*hash_free_key)(void *);

typedef struct st_hash {
  uint key_offset,key_length;		/* Length of key if const length */
  uint records, blength;
  uint flags;
  DYNAMIC_ARRAY array;				/* Place for hash_keys */
  hash_get_key get_key;
  void (*free)(void *);
  CHARSET_INFO *charset;
} HASH;

/* A search iterator state */
typedef uint HASH_SEARCH_STATE;

#define hash_init(A,B,C,D,E,F,G,H) _hash_init(A,B,C,D,E,F,G, H CALLER_INFO)
auto _hash_init(HASH *hash, CHARSET_INFO *charset,
		   uint default_array_elements, uint key_offset,
		   uint key_length, hash_get_key get_key,
		   void (*free_element)(void*), uint flags CALLER_INFO_PROTO) -> my_bool;
auto hash_free(HASH *tree) -> void;
auto my_hash_reset(HASH *hash) -> void;
auto hash_element(HASH *hash,uint idx) -> byte *;
auto hash_search(const HASH *info, const byte *key, uint length) -> gptr;
auto hash_first(const HASH *info, const byte *key, uint length,
                HASH_SEARCH_STATE *state) -> gptr;
auto hash_next(const HASH *info, const byte *key, uint length,
               HASH_SEARCH_STATE *state) -> gptr;
auto my_hash_insert(HASH *info,const byte *data) -> my_bool;
auto hash_delete(HASH *hash,byte *record) -> my_bool;
auto hash_update(HASH *hash,byte *record,byte *old_key,uint old_key_length) -> my_bool;
auto hash_replace(HASH *hash, HASH_SEARCH_STATE *state, byte *new_row) -> void;
auto hash_check(HASH *hash) -> my_bool;			/* Only in debug library */

#define hash_clear(H) bzero((char*) (H),sizeof(*(H)))
#define hash_inited(H) ((H)->array.buffer != 0)

//#ifdef	__cplusplus
//}
//#endif

} // namespace mysql_parser

#endif
