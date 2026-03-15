#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iterator>
#else
#include <stdarg.h>
#endif
#include <algorithm>
#include "myx_sql_tree_item.h"
#include "myx_lex_helpers.h"
#include <functional>
#include <assert.h>

#ifdef _WIN32
  #ifndef strcasecmp
    #define strcasecmp _stricmp
    #define strncasecmp strnicmp
  #endif
#endif

namespace mysql_parser
{

auto find_cstr_in_array_ci(const char *arr[], size_t arr_size, const char* str) -> const char* {
  for (size_t n= 0; n < arr_size; ++n)
    if (are_cstrings_eq_ci(arr[n], str))
      return arr[n];
  return NULL;
}
auto find_str_in_array_ci(const char *arr[], size_t arr_size, const std::string &str) -> const char* {
  return find_cstr_in_array_ci(arr, arr_size, str.c_str());
}


auto are_cstrings_eq(const char *str1, const char *str2, bool case_sensitive) -> bool {
  if (case_sensitive)
    return ((str1 == str2) ||
      (
        ((NULL != str1) && (NULL != str2)) &&
        (strlen(str1) == strlen(str2)) &&
        (0 == strcmp(str1, str2))
      ));
  else
    return are_cstrings_eq_ci(str1, str2);
}
auto are_strings_eq(const std::string &str1, const std::string &str2, bool case_sensitive) -> bool {
  return are_cstrings_eq(str1.c_str(), str2.c_str(), case_sensitive);
}


auto are_cstrings_eq_ci(const char *str1, const char *str2) -> bool {
  return ((str1 == str2) ||
    (
      ((NULL != str1) && (NULL != str2)) &&
      (toupper(str1[0]) == toupper(str2[0])) &&
      (strlen(str1) == strlen(str2)) &&
      (0 == strncasecmp(str1, str2, strlen(str1)))
    ));
}
auto are_strings_eq_ci(const std::string &str1, const std::string &str2) -> bool {
  return are_cstrings_eq_ci(str1.c_str(), str2.c_str());
}


MYX_PUBLIC_FUNC std::ostream& operator << (std::ostream& os, const SqlAstNode& item)
{
  if (item.value()[0] != '\0')
  {
    sql::symbol item_name= item.name();
    std::string item_value= item.value();
    os << "<elem name='" << (item_name ? sql::symbol_names[item_name] : "") << "' value='" << item_value.c_str() << "'>";
  }
  else
    os << "<elem name='" << item.name() << "'>";

  {
    SqlAstNode::SubItemList *subitems= item.subitems();
    if (subitems)
      for (SqlAstNode::SubItemList::const_iterator i= subitems->begin(), i_end= subitems->end(); i != i_end; ++i)
        os << *i;
  }

  os << "</elem>";

  return os;
}


std::list<SqlAstNode *> SqlAstStatics::_ast_nodes;
const SqlAstNode * SqlAstStatics::_tree= NULL;
const char * SqlAstStatics::_sql_statement= NULL;
bool SqlAstStatics::is_ast_generation_enabled= true;
static  std::shared_ptr<SqlAstTerminalNode> _last_terminal_node;
static  std::shared_ptr<SqlAstTerminalNode> _first_terminal_node;


auto SqlAstStatics::tree(const SqlAstNode *tree) -> void {
  _tree= tree;
  mysql_parser::tree= _tree;
}


auto SqlAstStatics::cleanup_ast_nodes() -> void {
  for (std::list<SqlAstNode*>::iterator i= _ast_nodes.begin(), i_end= _ast_nodes.end(); i != i_end; ++i)
    delete *i;
  _ast_nodes.clear();
  _tree= NULL;
  //_sql_statement= NULL;
}

auto SqlAstStatics::first_terminal_node() -> std::shared_ptr<SqlAstTerminalNode> {
  if (_first_terminal_node == NULL)
    first_terminal_node(std::shared_ptr<SqlAstTerminalNode>(new SqlAstTerminalNode));
  
  return _first_terminal_node;
}

auto SqlAstStatics::last_terminal_node() -> std::shared_ptr<SqlAstTerminalNode> {
  if (_last_terminal_node == NULL)
    last_terminal_node(std::shared_ptr<SqlAstTerminalNode>(new SqlAstTerminalNode));
  
  return _last_terminal_node;
}

auto SqlAstStatics::first_terminal_node(std::shared_ptr<SqlAstTerminalNode> value) -> void {
  _first_terminal_node = value;
}

auto SqlAstStatics::last_terminal_node(std::shared_ptr<SqlAstTerminalNode> value) -> void {
  _last_terminal_node = value;
}

SqlAstNode::SqlAstNode(sql::symbol name, const char *value, int value_length, int stmt_lineno, int stmt_boffset, int stmt_eoffset, SubItemList *items)
  :
_name(name),
_value((value) ? new std::string(value) : NULL),
_value_length(value_length),
_stmt_lineno(stmt_lineno),
_stmt_boffset(stmt_boffset),
_stmt_eoffset(stmt_eoffset),
_subitems(items)
{
  if (-1 != _stmt_eoffset && (_stmt_boffset + _value_length) > _stmt_eoffset)
    _stmt_eoffset= _stmt_boffset + _value_length;
}


SqlAstNode::~SqlAstNode()
{
}


auto SqlAstNode::value() const -> std::string {
  if (_value.get())
    return *_value;
  else if (_value_length)
    return std::string(SqlAstStatics::sql_statement() + _stmt_boffset, _value_length);
  else
    return "";
}


auto SqlAstNode::left_most_subitem() const -> const SqlAstNode * {
  return (_subitems) ? (*_subitems->begin())->left_most_subitem() : this;
}


auto SqlAstNode::right_most_subitem() const -> const SqlAstNode * {
  return (_subitems) ? (*_subitems->rend())->right_most_subitem() : this;
}


auto SqlAstNode::stmt_lineno() const -> int {
  if ((-1 == _stmt_lineno) && _subitems)
    return (*_subitems->begin())->stmt_lineno();
  return _stmt_lineno;
}


auto SqlAstNode::stmt_boffset() const -> int {
  if ((-1 == _stmt_boffset) && _subitems)
    return (*_subitems->begin())->stmt_boffset();
  return _stmt_boffset;
}


auto SqlAstNode::stmt_eoffset() const -> int {
  if ((-1 == _stmt_eoffset) && _subitems)
    return (*_subitems->rbegin())->stmt_eoffset();
  return _stmt_eoffset;
}


/*
Tries to find sequence of items begining from subitem (if specified, otherwise from 1st subitem).
Returns last item from found sequence.
*/
auto SqlAstNode::subseq__(const SqlAstNode *subitem, sql::symbol name, va_list args) const -> const SqlAstNode * {
  SqlAstNode::SubItemList::iterator i= _subitems->begin();
  SqlAstNode::SubItemList::iterator i_end= _subitems->end();

  // skip some elements if start subitem is specified
  if (subitem)
    i= std::find(i, i_end, subitem);

  for (; i != i_end; ++i)
  {
    subitem= *i;
    if (!subitem->name_equals(name))
      return NULL;
#ifdef __GNUC__
    name= (sql::symbol)va_arg(args, int);
#else
    name= va_arg(args, sql::symbol);
#endif
    if (!name)
      return subitem; // return last item from found sequence
  }

  return NULL;
}


auto SqlAstNode::subseq_(sql::symbol name, ...) const -> const SqlAstNode * {
  va_list args;
  va_start(args, name);
  const SqlAstNode * subitem= subseq__(NULL, name, args);
  va_end(args);
  return subitem;
}


auto SqlAstNode::subseq_(const SqlAstNode *subitem, sql::symbol name, ...) const -> const SqlAstNode * {
  va_list args;
  va_start(args, name);
  subitem= subseq__(subitem, name, args);
  va_end(args);
  return subitem;
}


/*
Tries to find sequence within whole range of children items starting from 'subitem' (if specified, otherwise from 1st subitem).
Returns last item from found sequence.
*/
auto SqlAstNode::find_subseq__(const SqlAstNode *subitem, sql::symbol name, va_list args) const -> const SqlAstNode * {
  SqlAstNode::SubItemList::iterator i= _subitems->begin();
  SqlAstNode::SubItemList::iterator i_end= _subitems->end();

  // skip some elements if start subitem is specified
  if (subitem)
    i= std::find(i, i_end, subitem);

  for (; i != i_end; ++i)
  {
    subitem= *i;
    if (subitem->name_equals(name) && (subitem= subseq__(subitem, name, args)))
      return subitem; // return last item from found sequence
  }

  return NULL;
}


auto SqlAstNode::find_subseq_(sql::symbol name, ...) const -> const SqlAstNode * {
  va_list args;
  va_start(args, name);
  const SqlAstNode * subitem= find_subseq__(NULL, name, args);
  va_end(args);
  return subitem;
}


auto SqlAstNode::find_subseq_(const SqlAstNode *subitem, sql::symbol name, ...) const -> const SqlAstNode * {
  va_list args;
  va_start(args, name);
  subitem= find_subseq__(subitem, name, args);
  va_end(args);
  return subitem;
}


/*
Tries to find sequence of items begining from subitem (if specified, otherwise from 1st subitem).
Returns last item from found sequence.
*/
auto SqlAstNode::check_words(sql::symbol words[], size_t words_count, const SqlAstNode *start_item) const -> const SqlAstNode * {
  const SqlAstNode *result= NULL;

  if (NULL != _subitems)
  {
    SqlAstNode::SubItemList::iterator i= _subitems->begin();
    SqlAstNode::SubItemList::iterator i_end= _subitems->end();

    // skip some elements if needed
    if (NULL != start_item)
      for (; (*i != start_item) && (i != i_end); ++i);

    // now check given sequence
    size_t n= 0;
    for (; n != words_count && i != i_end; ++i, ++n)
    {
      result= *i;
      if (!result->name_equals(words[n]))
        return NULL;
    }
    if (n < words_count)
      return NULL;
  }

  return result;
}


/*
Tries to find sequence within whole range of children items starting from 'subitem' (if specified, otherwise from 1st subitem).
Returns last item from found sequence.
*/
auto SqlAstNode::find_words(sql::symbol words[], size_t words_count, const SqlAstNode *start_item) const -> const SqlAstNode * {
  SqlAstNode::SubItemList::iterator i= _subitems->begin();
  SqlAstNode::SubItemList::iterator i_end= _subitems->end();

  // skip some elements if needed
  if (NULL != start_item)
    for (; (*i != start_item) && (i != i_end); ++i);

  // now try to find given sequence
  const SqlAstNode *item= NULL;
  size_t n= 0;
  for (; i != i_end; ++i)
  {
    item= *i;
    if (item->name_equals(words[n]))
    {
      if (words_count == ++n)
        break;
    }
    else
      n= 0;
  }

  return (words_count == n) ? item : NULL;
}


auto SqlAstNode::search_by_names(sql::symbol names[], size_t path_count) const -> const SqlAstNode * {
  const SqlAstNode *result= NULL;
  for (size_t n= 0; n < path_count; ++n)
    if ((result= subitem_by_name(names[n])))
      break;
  return result;
}


auto SqlAstNode::search_by_paths(sql::symbol * paths[], size_t path_count) const -> const SqlAstNode * {
  const SqlAstNode *result= NULL;
  for (size_t n= 0; n < path_count; ++n)
    if ((result= subitem_by_path(paths[n])))
      break;
  return result;
}


auto SqlAstNode::restore_sql_text(const std::string &sql_statement, const SqlAstNode *first_subitem, const SqlAstNode *last_subitem) const -> std::string {
  int boffset= first_subitem ? first_subitem->_stmt_boffset : -1;
  int eoffset= last_subitem ? last_subitem->_stmt_eoffset : -1;

  restore_sql_text(boffset, eoffset, first_subitem, last_subitem);

  if (-1 != boffset && -1 != eoffset)
  {
    std::string sql_text;
    sql_text.reserve(eoffset - boffset);
    std::copy(sql_statement.begin()+boffset, sql_statement.begin()+eoffset, std::back_inserter(sql_text));
    return sql_text;
  }

  return std::string();
}


auto SqlAstNode::restore_sql_text(int &boffset, int &eoffset, const SqlAstNode *first_subitem, const SqlAstNode *last_subitem) const -> void {
  if (-1 == boffset || (boffset > _stmt_boffset && -1 != _stmt_boffset))
    boffset= _stmt_boffset;
  if (-1 == eoffset || (eoffset < _stmt_eoffset && -1 != _stmt_eoffset))
    eoffset= _stmt_eoffset;
  if (NULL != _subitems)
  {
    SqlAstNode::SubItemList::const_iterator i= _subitems->begin();
    const SqlAstNode::SubItemList::const_iterator i_end= _subitems->end();
    if (first_subitem)
      for (; (i_end != i) && (*i != first_subitem); ++i);
    for (; (i != i_end) && (*i != last_subitem); ++i)
      (*i)->restore_sql_text(boffset, eoffset, NULL, NULL);
  }
}


// warning this sql has incorrect syntax
auto SqlAstNode::build_sql(std::string &sql_text) const -> void {
  if (_value_length)
  {
    sql_text.append(value());

    const char *nl_keywords[]= {"begin", "end", ";"};
    std::string item_value= value();
    if (find_cstr_in_array_ci(nl_keywords, ARR_CAPACITY(nl_keywords), item_value.c_str()))
      sql_text.append("\n");
    else
      sql_text.append(" ");
  }

  if (NULL != _subitems)
    for (SqlAstNode::SubItemList::const_iterator i= _subitems->begin(), i_end= _subitems->end(); i != i_end; ++i)
      (*i)->build_sql(sql_text);
}


auto SqlAstNode::subitem_(sql::symbol name, ...) const -> const SqlAstNode * {
  va_list args;
  va_start(args, name);
  const SqlAstNode *item= subitem__(name, args);
  va_end(args);
  return item;
}


auto SqlAstNode::subitem__(sql::symbol name, va_list args) const -> const SqlAstNode * {
  const SqlAstNode *item= this;

  while (name && item)
  {
    item= item->subitem_by_name(name);
    assert(sizeof(int) == sizeof(sql::symbol));
#ifdef __GNUC__
    name= (sql::symbol)va_arg(args, int);
#else
    name= va_arg(args, sql::symbol);
#endif
  }

  return item;
}


auto SqlAstNode::subitem_(int position, ...) const -> const SqlAstNode * {
  if ((0 <= position) && (_subitems->size() > (size_t)position))
  {
    SqlAstNode::SubItemList::const_iterator i= _subitems->begin();
    for (; 0 < position; --position) ++i;
    return *i;
  }
  return NULL;
}


auto SqlAstNode::subitem_by_name(sql::symbol name, const SqlAstNode *start_item) const -> const SqlAstNode * {
  if (!(_subitems))
    return NULL;

  SqlAstNode::SubItemList::const_iterator i= _subitems->begin();
  SqlAstNode::SubItemList::const_iterator i_end= _subitems->end();

  if (start_item)
    i= find(i, i_end, start_item);

  for (; i != i_end; ++i)
    if ((*i)->name_equals(name))
      return *i;

  return NULL;
}


auto SqlAstNode::subitem_by_name(sql::symbol name, size_t position) const -> const SqlAstNode * {
  if (!(_subitems))
    return NULL;

  if (_subitems->size() > position)
  {
    SqlAstNode::SubItemList::const_iterator i= _subitems->begin();
    SqlAstNode::SubItemList::const_iterator i_end= _subitems->end();

    for (; 0 < position; --position)
      ++i;

    for (; i != i_end; ++i)
      if ((*i)->name_equals(name))
        return *i;
  }

  return NULL;
}


auto SqlAstNode::rsubitem_by_name(sql::symbol name, size_t position) const -> const SqlAstNode * {
  if (_subitems->size() > position)
  {
    SqlAstNode::SubItemList::const_reverse_iterator i= _subitems->rbegin();
    SqlAstNode::SubItemList::const_reverse_iterator i_end= _subitems->rend();

    for (; 0 < position; --position)
      ++i;

    for (; i != i_end; ++i)
      if ((*i)->name_equals(name))
        return *i;
  }

  return NULL;
}


auto SqlAstNode::subitem_by_path(sql::symbol path[]) const -> const SqlAstNode * {
  const SqlAstNode *item= this;
  sql::symbol *name= path;

  while ((item) && *name)
    item= item->subitem_by_name(*name++);
  return item;
}


auto SqlAstNode::subitems_as_string(const char *delim) const -> char * {
  std::string to;

  if (_subitems)
  {
    const char *current_delim= "";
    for (SqlAstNode::SubItemList::const_iterator i= _subitems->begin(), i_end= _subitems->end(); i != i_end; ++i) 
    {
      SqlAstNode *subitem= *i;
      if (subitem->subitems()->size() > 0)
      {
        char *s= subitem->subitems_as_string(delim);
        to += current_delim;
        current_delim= delim;
        to += s;
        delete[] s;
      }
      else
      {
        to += current_delim;
        current_delim= delim;
        to += subitem->value();
      }
    }
  }

  return strcpy(new char[to.length()+1], to.c_str());
}


SqlAstNonTerminalNode::~SqlAstNonTerminalNode()
{
  // no need in this code after flat list of all allocated ast nodes was introduced, see SqlAstStatics::_ast_nodes
  //for (SubItemList::iterator i= _subitems.begin(), end= _subitems.end(); i != end; ++i)
  //  delete *i;
}


//extern "C"
//{

  extern auto new_ast_node(sql::symbol name) -> void * {
    SqlAstNode *node= SqlAstStatics::add_ast_node(new SqlAstNonTerminalNode(name));
    return node;
  }

  extern auto reuse_ast_node(void *node_, sql::symbol name) -> void * {
    return (node_) ? set_ast_node_name(node_, name) : new_ast_node(name);
  }

  extern auto set_ast_node_name(void *node_, sql::symbol name) -> void * {
    if (!node_)
      return node_;
    static_cast<SqlAstNode *>(node_)->set_name(name);
    return node_;
  }

  extern auto add_ast_child_node(void *parent_node_, void *child_node_) -> void {
    if (!parent_node_ || !child_node_)
      return;
    SqlAstNode *child_node= static_cast<SqlAstNode *>(child_node_);
    SqlAstNode *parent_node= static_cast<SqlAstNode *>(parent_node_);
    parent_node->subitems()->push_back(child_node);
  }

  extern auto merge_ast_child_nodes(void *dest_node_, void *src_node_) -> void {
    if (!dest_node_ || !src_node_)
      return;
    SqlAstNode::SubItemList *dest_list= static_cast<SqlAstNode *>(dest_node_)->subitems();
    SqlAstNode::SubItemList *src_list= static_cast<SqlAstNode *>(src_node_)->subitems();
    dest_list->splice(dest_list->end(), *src_list);
  }

  extern auto tree_item_dump_xml_to_file(const void *tree_item, const char *filename) -> void {
    const SqlAstNode* item= static_cast<const SqlAstNode *>(tree_item);
    std::ofstream os(filename);
    os << *item;
  }

//} // extern "C"

} // namespace mysql_parser

