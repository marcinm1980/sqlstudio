#include <stdio.h>
#include <sstream>
#ifdef _WIN32
#include "mysql_sql_parser_public_interface.h"
#endif
#include "myx_sql_parser_public_interface.h"
#include "myx_lex_helpers.h"
#include "myx_sql_parser.tab.hh"
#include "myx_sql_tree_item.h"

//#include "sql_lex.h"

namespace mysql_parser
{

std::istream* lex_input_stream= 0;
static std::string err_msg;
const void* tree= 0;
struct Lex_args lex_args;
extern auto MYSQLlex(void **arg, void *yyl) -> int;

auto yylex(void **yylval) -> int {
  //struct Lex_args *p= (struct Lex_args *)ptr_to_arg_pair;
  //return MYSQLlex(lex_args.arg1, lex_args.arg2); 
  
  int state= mysql_parser::MYSQLlex(yylval, mysql_parser::lex_args.arg2);
  //int state= myx_map_lexer_value(MYSQLlex(yylval, lex_args.arg2)); 
  //return state == END_OF_INPUT ? 0 : state;
  return state;
}

auto yyerror(const char *msg) -> void { mysql_parser::err_msg= msg; }
/*
int yywrap() { return 1; }  // stop after EOF

void yy_custom_input(char *buf, int* result, int max_size) 
{
  mysql_parser::lex_input_stream->read(buf, max_size);
  *result= mysql_parser::lex_input_stream->gcount();
}

int yy_token_match(int token, const char *value)
{
  return token;
}

int yy_unknown_token(const char *value)
{
  //printf("error %s", value);
  return 0;
}
*/

auto myx_get_err_msg() -> MYX_PUBLIC_FUNC const std::string & {
  return err_msg;
}

auto myx_get_parser_tree() -> MYX_PUBLIC_FUNC const void * {
  return tree;
}

auto myx_set_parser_input(std::istream *sqlstream) -> MYX_PUBLIC_FUNC void {
  lex_input_stream= sqlstream;
}

auto myx_set_parser_source(const char *sql) -> MYX_PUBLIC_FUNC void {
  lex_input_stream= new std::istringstream(sql);
}

auto myx_set_parser_source(std::istream *sqlstream) -> MYX_PUBLIC_FUNC void {
  lex_input_stream= sqlstream;
}

auto myx_free_parser_source(void) -> MYX_PUBLIC_FUNC void {
  delete lex_input_stream;
  SqlAstStatics::cleanup_ast_nodes();
}

auto myx_parse(void) -> MYX_PUBLIC_FUNC void {
  err_msg.clear();
  yyparse();
}


// server replacement routines
//
//extern "C" {

extern auto strmake_root(const char *str, unsigned int len) -> char * {
  char *pos;
  if ((pos=(char *)malloc(len+1)))
  {
    memcpy(pos,str,len);
    pos[len]=0;
  }
  return pos;
}

extern auto strdup_root(const char *str) -> char * {
  return strmake_root(str, (unsigned int) strlen(str));
} 

extern auto alloc_root(unsigned int size) -> char * {
  return (char *)malloc(size);
}

extern auto memdup_root(const char *str, unsigned int len) -> char * {
  char *pos;
  if ((pos=alloc_root(len)))
    memcpy(pos,str,len);
  return pos;
}

//} // extern C

} // namespace mysql_parser
