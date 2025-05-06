#include "base/string_utilities.h"
#include "base/file_functions.h"
#include "base/log.h"


#include <stdexcept>
#include <functional>
#include <locale>
#include <algorithm>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <fstream>
//#include <boost/locale/encoding_utf.hpp>
#include <cstdarg>
#include <filesystem>


DEFAULT_LOG_DOMAIN(DOMAIN_BASE);

namespace base {

#ifdef _MSC_VER

  //--------------------------------------------------------------------------------------------------

  thread_local static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf16Converter;
  thread_local static std::wstring_convert<std::codecvt_utf8<__int32>, __int32> utf32Converter;

  /**
   * Converts an UTF-8 encoded string to an UTF-16 string.
   */
  std::wstring string_to_wstring(const std::string &s) {
    if (sizeof(wchar_t) > 2) {
      auto utf32String = utf32Converter.from_bytes(s);
      return std::wstring(utf32String.begin(), utf32String.end());
    } else
      return utf16Converter.from_bytes(s);
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Converts an UTF-16 encoded string to an UTF-8 string.
   */
  std::string wstring_to_string(const std::wstring &s) {
    if (sizeof(wchar_t) > 2)
      return utf32Converter.to_bytes((__int32 *)s.c_str());
    else
      return utf16Converter.to_bytes(s);
  }

  //--------------------------------------------------------------------------------------------------

  std::wstring path_from_utf8(const std::string &s) {
    return string_to_wstring(s);
  }

#else

  using boost::locale::conv::utf_to_utf;

  std::wstring string_to_wstring(const std::string &str) {
    return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
  }

  //--------------------------------------------------------------------------------------------------

  std::string wstring_to_string(const std::wstring &str) {
    if (sizeof(wchar_t) > 2)
      return utf_to_utf<char>((int32_t *)str.c_str(), (int32_t *)str.c_str() + str.size());
    else
      return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
  }

  //--------------------------------------------------------------------------------------------------

  std::string path_from_utf8(const std::string &s) {
    return s;
  }

#endif

  //--------------------------------------------------------------------------------------------------

  std::string string_to_path_for_open(const std::string &s) {
#ifdef _MSC_VER
    // Convert UTF-8 string to wide string
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    if (wideSize == 0) {
      throw std::runtime_error("Failed to convert UTF-8 to wide string.");
    }

    std::vector<wchar_t> wideBuffer(wideSize);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wideBuffer.data(), wideSize);

    // Get the short path name
    int shortPathSize = GetShortPathNameW(wideBuffer.data(), nullptr, 0);
    if (shortPathSize == 0) {
      return s; // Return the original string if short path name retrieval fails
    }

    std::vector<wchar_t> shortPathBuffer(shortPathSize);
    if (GetShortPathNameW(wideBuffer.data(), shortPathBuffer.data(), shortPathSize) == 0) {
      return s; // Return the original string if short path name retrieval fails
    }

    // Convert wide string back to UTF-8
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, shortPathBuffer.data(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Size == 0) {
      throw std::runtime_error("Failed to convert wide string to UTF-8.");
  }

    std::vector<char> utf8Buffer(utf8Size);
    WideCharToMultiByte(CP_UTF8, 0, shortPathBuffer.data(), -1, utf8Buffer.data(), utf8Size, nullptr, nullptr);

    return std::string(utf8Buffer.data());
#else
    return s; // On non-Windows platforms, return the original string
#endif
  }

  //--------------------------------------------------------------------------------------------------

  inline bool is_invalid_filesystem_char(int ch) {
    static const char invalids[] = "/?<>\\:*|\"^";

    return memchr(invalids, ch, sizeof(invalids) - 1) != NULL;
  }

  std::string sanitize_file_name(const std::string &s) {
    static const char *invalid_filenames[] = {"com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8",
                                              "com9", "lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7",
                                              "lpt8", "lpt9", "con",  "nul",  "prn",  ".",    "..",   NULL};
    std::string out;

    for (std::string::const_iterator c = s.begin(); c != s.end(); ++c) {
      // utf-8 has the high-bit = 1, so we just copy those verbatim
      if ((unsigned char)*c >= 128 || isalnum(*c) || (ispunct(*c) && !is_invalid_filesystem_char(*c)))
        out.push_back(*c);
      else
        out.push_back('_');
    }

    // not valid under windows
    if (!out.empty() && (out[out.size() - 1] == ' ' || out[out.size() - 1] == '.'))
      out[out.size() - 1] = '_';

    for (const char **fn = invalid_filenames; *fn; ++fn) {
      if (strcmp(out.c_str(), *fn) == 0) {
        out.append("_");
        break;
      }
    }

    return out;
  }

  //--------------------------------------------------------------------------------------------------

  std::string trim_right(const std::string &s, const std::string &t) {
    std::string d(s);
    std::string::size_type i(d.find_last_not_of(t));
    if (i == std::string::npos)
      return "";
    else
      return d.erase(d.find_last_not_of(t) + 1);
  }

  //--------------------------------------------------------------------------------------------------

  std::string trim_left(const std::string &s, const std::string &t) {
    std::string d(s);
    return d.erase(0, s.find_first_not_of(t));
  }

  //--------------------------------------------------------------------------------------------------

  std::string trim(const std::string &s, const std::string &t) {
    std::string d(s);
    return trim_left(trim_right(d, t), t);
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Simple case conversion routine, which returns a new string.
   * Note: converting to lower can be wrong when the returned string is used for string comparison,
   * because in some cultures letter cases are more complicated. Use string_compare instead in such cases.
   */
  std::string tolower(const std::string &s) {
    std::string result;
    result.reserve(s.size()); // Reserve space to avoid reallocations

    std::locale loc; // Use the default locale
    for (char c : s) {
      result.push_back(std::tolower(c, loc));
    }

    return result;
  }

  //--------------------------------------------------------------------------------------------------

  std::string toupper(const std::string &s) {
    std::string result;
    result.reserve(s.size()); // Reserve space to avoid reallocations

    std::locale loc; // Use the default locale
    for (char c : s) {
      result.push_back(std::toupper(c, loc));
    }

    return result;
  }

  //--------------------------------------------------------------------------------------------------

  std::string truncate_text(const std::string &s, int max_length) {
    if (static_cast<int>(s.length()) > max_length) {
      std::string shortened = s.substr(0, max_length);

      // Ensure we don't cut off in the middle of a multibyte UTF-8 character
      size_t lastValidPos = shortened.find_last_of("\xC0-\xFD");
      if (lastValidPos != std::string::npos && lastValidPos + 1 > max_length) {
        shortened = shortened.substr(0, lastValidPos);
      }
      shortened.append("...");
      return shortened;
    }
    return s;
  }

  //--------------------------------------------------------------------------------------------------

  std::string sanitize_utf8(const std::string &s) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    try {
      // Attempt to convert the string to UTF-32 and back to UTF-8
      std::u32string utf32 = converter.from_bytes(s);
      return converter.to_bytes(utf32);
    }
    catch (const std::range_error&) {
      // If invalid UTF-8 is encountered, truncate the string up to the valid part
      size_t valid_length = converter.converted();
      return s.substr(0, valid_length);
    }
  }

  //--------------------------------------------------------------------------------------------------

  std::vector<std::string> split(const std::string &s, const std::string &sep, int count) {
    std::vector<std::string> parts;
    std::string ss = s;

    std::string::size_type p;

    if (s.empty())
      return parts;

    if (count == 0)
      count = -1;

    p = ss.find(sep);
    while (!ss.empty() && p != std::string::npos && (count < 0 || count > 0)) {
      parts.push_back(ss.substr(0, p));
      ss = ss.substr(p + sep.size());

      --count;
      p = ss.find(sep);
    }
    parts.push_back(ss);

    return parts;
  }

  //--------------------------------------------------------------------------------------------------

  std::vector<std::string> split_by_set(const std::string &s, const std::string &separator_set, int count) {
    std::vector<std::string> parts;
    std::string ss = s;

    std::string::size_type p;

    if (s.empty())
      return parts;

    if (count == 0)
      count = -1;

    p = ss.find_first_of(separator_set);
    while (!ss.empty() && p != std::string::npos && (count < 0 || count > 0)) {
      parts.push_back(ss.substr(0, p));
      ss = ss.substr(p + 1);

      --count;
      p = ss.find_first_of(separator_set);
    }
    parts.push_back(ss);

    return parts;
  }

  //--------------------------------------------------------------------------------------------------

  static void findUntil(const char elem, const std::string &str, const int sep, std::string::size_type &p,
                        std::string::size_type &pe, std::string::size_type &end, std::vector<std::string> &parts) {
    // keep going until we find closing '
    while (pe < end) {
      auto it = str[pe++];
      if (it == elem) {
        if (pe < end && str[pe] == elem)
          pe++;
        else
          break;
      } else if (it == '\\') {
        if (pe < end)
          pe++;
      }
    }
    parts.push_back(str.substr(p, pe - p));
    p = pe;
    // skip whitespace
    while (p < end && (str[p] == ' ' || str[p] == '\t' || str[p] == '\r' || str[p] == '\n'))
      p++;
    if (p < end) {
      if (str[p] != sep)
        logDebug("Error splitting string list\n");
      else
        p++;
    }
  }

  std::vector<std::string> split_token_list(const std::string &s, int sep) {
    std::vector<std::string> parts;
    std::string ss = s;

    std::string::size_type end = s.size(), pe, p = 0;

    {
      bool empty_pending = true;
      while (p < end) {
        empty_pending = false;
        switch (s[p]) {
          case '\'':
            pe = p + 1;
            findUntil('\'', s, sep, p, pe, end, parts);
            break;

          case '"':
            pe = p + 1;
            findUntil('"', s, sep, p, pe, end, parts);
            break;

          case ' ':
          case '\t':
            p++;
            break;

          default:
            // skip until separator
            pe = p;
            while (pe < end) {
              if (s[pe] == sep) {
                empty_pending = true;
                break;
              }
              pe++;
            }
            parts.push_back(trim_right(s.substr(p, pe - p)));
            p = pe + 1;
            // skip whitespace
            while (p < end && (s[p] == ' ' || s[p] == '\t' || s[p] == '\r' || s[p] == '\n'))
              p++;
            break;
        }
      }
      if (empty_pending)
        parts.push_back("");
    }

    return parts;
  }

  //--------------------------------------------------------------------------------------------------

  bool partition(const std::string &s, const std::string &sep, std::string &left, std::string &right) {
    std::string::size_type p = s.find(sep);
    if (p != std::string::npos) {
      left = s.substr(0, p);
      right = s.substr(p + sep.size());
      return true;
    }
    left = s;
    right = "";
    return false;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Returns the index of the given string in the given vector or -1 if not found.
   */
  int index_of(const std::vector<std::string> &list, const std::string &s) {
    std::vector<std::string>::const_iterator location = std::find(list.begin(), list.end(), s);
    if (location == list.end())
      return -1;
    return (int)(location - list.begin());
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Returns a string containing all characters beginning at "start" from the given string "id", which form
   * a valid, unqualified identifier. The returned identifier does not contain any quoting anymore.
   * Note: this function is UTF-8 safe as it skips over all characters except some which are guaranteed
   *       not to be part of any valid UTF-8 sequence.
   *
   * @param id The string to examine.
   * @param start The start position to search from.
   *
   * @result Returns the first found identifier starting at "start" or an empty string if nothing was
   *         found. Parameter "start" points to the first character after the found identifier.
   */
  std::string get_identifier(const std::string &id, std::string::const_iterator &start) {
    std::string::const_iterator token_end = id.end();
    bool is_symbol_quoted = false;
    for (std::string::const_iterator i = start, i_end = token_end; i != i_end; ++i) {
      if (i_end != token_end)
        break;
      switch (*i) {
        case '.':
          if (!is_symbol_quoted)
            token_end = i;
          break;
        case ' ':
          if (!is_symbol_quoted)
            token_end = i;
          break;
        case '\'':
        case '"':
        case '`':
          if (*i == *start) {
            if (i != start)
              token_end = i + 1;
            else
              is_symbol_quoted = true;
          }
          break;
      }
    }

    if (token_end - start < 2)
      is_symbol_quoted = false;
    std::string result(start, token_end);
    start = token_end;
    if (is_symbol_quoted)
      return result.substr(1, result.size() - 2);

    return result;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Splits the given string into identifier parts assuming a format as allowed by the MySQL syntax for
   * qualified identifiers, e.g. part1.part2.part3 (any of the parts might be quoted).
   * In addition to the traditional syntax also these enhancements are supported:
   * - Unlimited level of nesting.
   * - Quoting might be done using single quotes, double quotes and back ticks.
   *
   * If an identifier is not separated by a dot from the rest of the input then this is considered
   * invalid input and ignored. Only identifiers found until that syntax violation are returned.
   */
  std::vector<std::string> split_qualified_identifier(const std::string &id) {
    std::vector<std::string> result;
    std::string::const_iterator iterator = id.begin();
    std::string token;
    do {
      token = get_identifier(id, iterator);
      if (token == "")
        break;
      result.push_back(token);
    } while ((iterator != id.end()) && (*iterator++ == '.'));

    return result;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Removes the first path part from @path and returns this part as well as the shortend path.
   */
  std::string pop_path_front(std::string &path) {
    std::string::size_type p = path.find('/');
    std::string res;
    if (p == std::string::npos || p == path.length() - 1) {
      res = path;
      path.clear();
      return res;
    }
    res = path.substr(0, p);
    path = path.substr(p + 1);
    return res;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Removes the last path part from @path and returns this part as well as the shortend path.
   */
  std::string pop_path_back(std::string &path) {
    std::string::size_type p = path.rfind('/');
    std::string res;
    if (p == std::string::npos || p == path.length() - 1) {
      res = path;
      path.clear();
      return res;
    }
    res = path.substr(p + 1);
    path = path.substr(0, p);
    return res;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Helper routine to format a string into an STL string using the printf parameter syntax.
   */
  std::string strfmt(const char *fmt, ...) {
    if (!fmt) {
      throw std::invalid_argument("Format string cannot be null.");
    }

    va_list args;
    va_start(args, fmt);

    // Determine the size of the formatted string
    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = std::vsnprintf(nullptr, 0, fmt, argsCopy);
    va_end(argsCopy);

    if (size < 0) {
      va_end(args);
      throw std::runtime_error("Error formatting string.");
    }

    // Create a buffer for the formatted string
    std::vector<char> buffer(size + 1);
    std::vsnprintf(buffer.data(), buffer.size(), fmt, args);
    va_end(args);

    return std::string(buffer.data());
  }

  //--------------------------------------------------------------------------------------------------

  BASELIBRARY_PUBLIC_FUNC std::string sizefmt(int64_t s, bool metric) {
    float one_kb;
    const char *unit;
    if (metric) {
      one_kb = 1000;
      unit = "B";
    } else {
      one_kb = 1024;
      unit = "iB"; // http://en.wikipedia.org/wiki/Binary_prefix
    }

    if (s < one_kb)
      return strfmt("%iB", (int)s);
    else {
      float value = s / one_kb;
      if (value < one_kb)
        return strfmt("%.02fK%s", value, unit);
      else {
        value /= one_kb;
        if (value < one_kb)
          return strfmt("%.02fM%s", value, unit);
        else {
          value /= one_kb;
          if (value < one_kb)
            return strfmt("%.02fG%s", value, unit);
          else {
            value /= one_kb;
            if (value < one_kb)
              return strfmt("%.02fT%s", value, unit);
            else
              return strfmt("%.02fP%s", value / one_kb, unit);
          }
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Helper routine to strip a string into an STL string using the printf parameter syntax.
   */
  std::string strip_text(const std::string &text, bool left, bool right) { // TODO sigc rewrite it in std/boost way
    std::locale loc;
    std::function<bool(std::string::value_type)> is_space =
      std::bind(&std::isspace<std::string::value_type>, std::placeholders::_1, loc);

    std::string::const_iterator l_edge =
      !left ? text.begin()
            : std::find_if(text.begin(), text.end(),
                           std::bind(std::logical_not<bool>(), std::bind(is_space, std::placeholders::_1)));
    std::string::const_reverse_iterator r_edge =
      !right ? text.rbegin()
             : std::find_if(text.rbegin(), text.rend(),
                            std::bind(std::logical_not<bool>(), std::bind(is_space, std::placeholders::_1)));

    return std::string(l_edge, r_edge.base());
  }

  //--------------------------------------------------------------------------------------------------

  std::string replaceVariable(const std::string& format, const std::string& variable, const std::string& value) {
    std::string result = format;
    std::string::size_type pos;

    for (;;) {
      std::string s;
      std::string::size_type end;

      pos = result.find(variable.substr(0, variable.size() - 1));
      if (pos == std::string::npos)
        break;

      end = result.find('%', pos + 1);
      if (end == std::string::npos) // bad format
        break;

      s = result.substr(pos + 1, end - pos - 1);

      std::string::size_type filter_pos = s.find("|");
      std::string filtered_value = value;

      if (filter_pos == std::string::npos) {
        if (s.length() != variable.length() - 2)
          break;
      }
      else if (filter_pos != variable.length() - 2)
        break;
      else {
        std::string filter = s.substr(filter_pos + 1, s.size() - filter_pos);

        if (filter == "capitalize") {
          // Capitalize the first character and append the rest
          if (!value.empty()) {
            filtered_value[0] = std::toupper(value[0], std::locale());
            std::transform(value.begin() + 1, value.end(), std::back_inserter(filtered_value), [](char c) {
              return std::tolower(c, std::locale());
              });
          }
        }
        else if (filter == "uncapitalize") {
          // Uncapitalize the first character and append the rest
          if (!value.empty()) {
            filtered_value[0] = std::tolower(value[0], std::locale());
            std::copy(value.begin() + 1, value.end(), std::back_inserter(filtered_value));
          }
        }
        else if (filter == "lower") {
          // Convert the entire string to lowercase
          std::transform(value.begin(), value.end(), filtered_value.begin(), [](char c) {
            return std::tolower(c, std::locale());
            });
        }
        else if (filter == "upper") {
          // Convert the entire string to uppercase
          std::transform(value.begin(), value.end(), filtered_value.begin(), [](char c) {
            return std::toupper(c, std::locale());
            });
        }
      }
      result = result.substr(0, pos).append(filtered_value).append(result.substr(end + 1));
    }

    return result;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Add the given extension to the filename, if necessary.
   *
   */
  std::string normalize_path_extension(std::string filename, std::string extension) {
    if (!extension.empty() && !filename.empty()) {
      std::string::size_type p = filename.rfind('.');
      std::string old_extension = p != std::string::npos ? filename.substr(p) : "";

      if (old_extension.find('/') != std::string::npos || old_extension.find('\\') != std::string::npos)
        old_extension.clear();

      if (!extension.empty() && extension[0] != '.')
        extension = "." + extension;

      if (old_extension.empty())
        filename.append(extension);
      else {
        if (old_extension != extension)
          filename = filename.substr(0, p).append(extension);
      }
    }
    return filename;
  }

  /**
   * Removes all unnecessary path separators as well as "./" combinations.
   * If there is a parent-dir entry (../) then this as well as the directly prefacing
   * dir entry is removed.
   */
  std::string normalize_path(const std::string path) {
    // First convert all separators to the one that is used on the platform (no mix)
    // and ease so at the same time further processing here.
    std::string result;
    std::string separator(1, std::filesystem::path::preferred_separator);

    result = path;
    replaceStringInplace(result, "\\", separator);
    replaceStringInplace(result, "/", separator);

    std::string double_separator = separator + separator;
    while (result.find(double_separator) != std::string::npos)
      replaceStringInplace(result, double_separator, separator);

    // Sanity check. Return *after* we have converted the slashs. This is part of the normalization.
    if (result.size() < 2)
      return result;

    std::vector<std::string> parts = split(result, separator);

    // Construct result backwards while examining the path parts.
    result = "";
    int pending_count = 0;
    for (ssize_t i = parts.size() - 1; i >= 0; i--) {
      if (parts[i].compare(".") == 0)
        // References to the current directory can be removed without further change.
        continue;

      if (parts[i].compare("..") == 0) {
        // An entry that points back to the parent dir.
        // Ignore this and keep track for later removal of the parent dir.
        pending_count++;
      } else if (pending_count > 0) {
        // If this is a normal dir entry and we have pending parent-dir redirections
        // then go one step up by removing (ignoring) this entry.
        pending_count--;
      } else
        result = separator + parts[i] + result;
    }

    // Don't return the leading separator.
    return result.substr(1);
  }

  std::string expand_tilde(const std::string& path) {
    if (!path.empty() && path[0] == '~' && (path.size() == 1 || path[1] == std::filesystem::path::preferred_separator)) {
      const char* homedir = std::getenv("HOME");
#ifdef _WIN32
      if (!homedir) {
        homedir = std::getenv("USERPROFILE"); // Fallback for Windows
      }
#endif
      if (!homedir) {
        throw std::runtime_error("Home directory environment variable is not set.");
      }
      return std::string(homedir).append(path.substr(1));
    }
    return path;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Checks the input for characters not allowed in the file system and converts them to underscore.
   */
  std::string make_valid_filename(const std::string &name) {
    std::string result;
    std::string illegal_chars = "\\/:?\"<>|*";
    for (std::string::const_iterator iterator = name.begin(); iterator != name.end(); ++iterator) {
      if (illegal_chars.find(*iterator) != std::string::npos)
        result += '_';
      else
        result += *iterator;
    }
    return result;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Get a string containing the 'len' left most characters.
   */
  std::string left(const std::string &s, size_t len) {
    return s.substr(0, len);
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Get a string containing the 'len' right most characters.
   */
  std::string right(const std::string &s, size_t len) {
    if (len > s.size())
      len = s.size();
    if (len < 1)
      return "";

    return s.substr(std::max(s.length() - len, (size_t)0));
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Tests if s begins with part.
   */
  bool hasPrefix(const std::string &s, const std::string &part) {
    return s.compare(0, part.length(), part) == 0;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Tests if s ends with part.
   */
  bool hasSuffix(const std::string &s, const std::string &part) {
    int start_at = (int)s.length() - (int)part.length();

    // If start_at < 0 then the search string is bigger then the source, so the results is false.
    // On the other hand, if it starts after the end, something went wrong...
    if (start_at < 0 || start_at > (int)s.length())
      return false;

    return s.compare(start_at, std::string::npos, part) == 0;
  }

  //--------------------------------------------------------------------------------------------------

  void replaceStringInplace(std::string &value, const std::string &search, const std::string &replacement) {
    std::string::size_type next;

    for (next = value.find(search); next != std::string::npos; next = value.find(search, next)) {
      value.replace(next, search.length(), replacement);
      next += replacement.length();
    }
  }

  //--------------------------------------------------------------------------------------------------

  std::string replaceString(const std::string &s, const std::string &from, const std::string &to) {
    std::string::size_type p;
    std::string ss, res;

    ss = s;
    p = ss.find(from);
    while (p != std::string::npos) {
      if (p > 0)
        res.append(ss.substr(0, p)).append(to);
      else
        res.append(to);
      ss = ss.substr(p + from.size());
      p = ss.find(from);
    }
    res.append(ss);

    return res;
  }

  //--------------------------------------------------------------------------------------------------

  void setTextFileContent(const std::string& filename, const std::string& data) {
#ifdef _MSC_VER
    // Opening a file in text mode will automatically convert \n to \r\n.
    FILE* f = base_fopen(filename.c_str(), "w+t");
    if (!f)
      throw std::runtime_error(std::strerror(errno)); // Replaced g_strerror with std::strerror

    size_t bytes_written = fwrite(data.data(), 1, data.size(), f);
    fclose(f);
    if (bytes_written != data.size())
      throw std::runtime_error(std::strerror(errno)); // Replaced g_strerror with std::strerror
#else
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error(std::strerror(errno)); // Replaced g_strerror with std::strerror
    }

    file.write(data.data(), data.size());
    if (!file) {
      throw std::runtime_error("Failed to write the entire content to the file.");
    }
#endif
    }


  //--------------------------------------------------------------------------------------------------

  /**
   * Reads text data from the given file (file name encoded as utf-8) and returns the content as utf-8.
   * It can read ASCII/ANSI, utf-8 and utf-16 files (LE only) with and without BOM (BOM not included in result).
   */
  std::string getTextFileContent(const std::string &filename) {
    enum Encoding { ANSI, UTF8, UTF16LE } encoding = ANSI;

    std::string result;
#ifdef _MSC_VER
    std::ifstream stream(string_to_wstring(filename).c_str(), std::ios::binary);
#else
    std::ifstream stream(filename.c_str(), std::ifstream::binary);
#endif

    if (!stream.is_open() || stream.eof())
      return "";

    int ch1 = stream.get();
    int ch2 = stream.get();
    if (ch1 == 0xff && ch2 == 0xfe)
      encoding = UTF16LE;
    else if (ch1 == 0xfe && ch2 == 0xff)
      return "UTF-16BE not supported";
    else {
      int ch3 = stream.get();
      if (ch1 == 0xef && ch2 == 0xbb && ch3 == 0xbf)
        encoding = UTF8;
      else
        stream.seekg(0);
    }

    std::string tmp;
    stream.seekg(0, std::ios::end);
    tmp.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);

    tmp.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    switch (encoding) {
      case UTF16LE:
        return wstring_to_string(std::wstring((const wchar_t *)tmp.data()));
      default:
        return tmp;
    }
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Escape a string to be used in a SQL query
   * Same code as used by mysql. Handles null bytes in the middle of the string.
   * If wildcards is true then _ and % are masked as well.
   */
  std::string escape_sql_string(const std::string &s, bool wildcards) {
    std::string result;
    result.reserve(s.size());

    for (std::string::const_iterator ch = s.begin(); ch != s.end(); ++ch) {
      char escape = 0;

      switch (*ch) {
        case 0: /* Must be escaped for 'mysql' */
          escape = '0';
          break;
        case '\n': /* Must be escaped for logs */
          escape = 'n';
          break;
        case '\r':
          escape = 'r';
          break;
        case '\\':
          escape = '\\';
          break;
        case '\'':
          escape = '\'';
          break;
        case '"': /* Better safe than sorry */
          escape = '"';
          break;
        case '\032': /* This gives problems on Win32 */
          escape = 'Z';
          break;
        case '_':
          if (wildcards)
            escape = '_';
          break;
        case '%':
          if (wildcards)
            escape = '%';
          break;
      }
      if (escape) {
        result.push_back('\\');
        result.push_back(escape);
      } else
        result.push_back(*ch);
    }
    return result;
  }

  /**
   * Escape a string to be used in a JSON
   */
  std::string escape_json_string(const std::string &s) {
    std::string result;
    result.reserve(s.size());
    for (auto ch : s) {
      char escape = 0;
      switch (ch) {
        case '"':
          escape = '"';
          break;
        case '\\':
          escape = '\\';
          break;
        case '\b':
          escape = 'b';
          break;
        case '\f':
          escape = 'f';
          break;
        case '\n':
          escape = 'n';
          break;
        case '\r':
          escape = 'r';
          break;
        case '\t':
          escape = 't';
          break;
        default:
          break;
      }
      if (escape) {
        result.push_back('\\');
        result.push_back(escape);
      } else
        result.push_back(ch);
    }
    return result;
  }

  /**
   * Removes repeated quote chars and supported escape sequences from the given string.
   * Invalid escape sequences are handled like in the server, by dropping the backslash and
   * using the wrong char as normal char.
   * The outer quoting stays intact and is not removed.
   */
  std::string unescape_sql_string(const std::string &s, char quote_char) {
    // Early out if the string is simply empty but quoted.
    if (s.size() == 2 && s[0] == quote_char && s[1] == quote_char)
      return s;

    std::string result;
    result.reserve(s.size());

    bool pendingQuote = false;
    bool pendingEscape = false;
    for (auto c : s) {
      if (!pendingEscape && c == quote_char) {
        if (pendingQuote)
          pendingQuote = false;
        else {
          pendingQuote = true;
          continue;
        }
      } else {
        if (pendingQuote) {
          pendingQuote = false;
          result.push_back(quote_char);
        }

        if (pendingEscape) {
          pendingEscape = false;
          switch (c) {
            case 'n':
              c = '\n';
              break;
            case 't':
              c = '\t';
              break;
            case 'r':
              c = '\r';
              break;
            case 'b':
              c = '\b';
              break;
            case '0':
              c = 0;
              break; // ASCII null
            case 'Z':
              c = '\032';
              break; // Win32 end of file
          }
        } else if (c == '\\') {
          pendingEscape = true;
          continue;
        }
      }
      result.push_back(c);
    }

    if (pendingQuote)
      result.push_back(quote_char);
    if (pendingEscape)
      result.push_back('\\');

    return result;
  }

  //--------------------------------------------------------------------------------------------------

  // NOTE: This is not the same as escape_sql_string, as embedded ` must be escaped as ``, not \`
  // and \ ' and " must not be escaped
  std::string escape_backticks(const std::string &s) {
    std::string result;
    result.reserve(s.size());

    for (std::string::const_iterator ch = s.begin(); ch != s.end(); ++ch) {
      char escape = 0;

      switch (*ch) {
        case 0: /* Must be escaped for 'mysql' */
          escape = '0';
          break;
        case '\n': /* Must be escaped for logs */
          escape = 'n';
          break;
        case '\r':
          escape = 'r';
          break;
        case '\032': /* This gives problems on Win32 */
          escape = 'Z';
          break;
        case '`':
          // special case
          result.push_back('`');
          break;
      }
      if (escape) {
        result.push_back('\\');
        result.push_back(escape);
      } else
        result.push_back(*ch);
    }
    return result;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Parses the given command line (which must be a usual mysql start command) and extracts the
   * value for the given parameter. The function can only return options of the form "option-name = option-value"
   * (both quoted and unquoted).
   */
  std::string extract_option_from_command_line(const std::string &option, const std::string &command_line) {
    std::string result;
    size_t position = command_line.find(option);
    if (position != std::string::npos) {
      position += option.size(); // Skip option name and find equal sign.
      while (position < command_line.size() && command_line[position] != '=')
        position++;

      if (command_line[position] == '=') {
        position++;

        // Skip any white space.
        while (position < command_line.size() && command_line[position] == ' ')
          position++;

        char terminator;
        if (command_line[position] == '"' || command_line[position] == '\'')
          terminator = command_line[position++];
        else
          terminator = ' ';

        size_t end_position = command_line.find(terminator, position);
        if (end_position == std::string::npos) {
          // Terminator not found means the string was either not properly terminated (if quoted)
          // or contains no space char. In this case take everything we can get.
          if (terminator != ' ')
            position++;
          result = command_line.substr(position);
        } else
          result = command_line.substr(position, end_position - position);
      }
    }
    return result;
  }

  //--------------------------------------------------------------------------------------------------

  bool parse_font_description(const std::string& fontspec, std::string& font, float& size, bool& bold, bool& italic) {
    std::vector<std::string> parts = split(fontspec, " ");
    font = fontspec;
    size = 12;
    bold = false;
    italic = false;

    if (parts.empty())
      return false;

    for (auto iter = parts.begin(); iter != parts.end(); ++iter) {
      float size_check = 0;
      if (sscanf(iter->c_str(), "%f", &size_check) == 1) {
        size = size_check;
        parts.erase(iter);
        break;
      }
    }

    auto case_insensitive_compare = [](const std::string& a, const std::string& b) {
      return a.size() == b.size() &&
        std::equal(a.begin(), a.end(), b.begin(), [](char c1, char c2) {
        return std::tolower(c1) == std::tolower(c2);
          });
      };

    for (int i = 0; i < 2 && !parts.empty(); i++) {
      if (case_insensitive_compare(parts.back(), "bold")) {
        bold = true;
        parts.pop_back();
      }

      if (!parts.empty() && case_insensitive_compare(parts.back(), "italic")) {
        italic = true;
        parts.pop_back();
      }
    }

    if (!parts.empty()) {
      font = parts[0];
      for (unsigned int i = 1; i < parts.size(); i++)
        font += " " + parts[i];
    }
    return true;
  }


  //--------------------------------------------------------------------------------------------------

  std::string unquote_identifier(const std::string &identifier) {
    int start = 0;
    int size = (int)identifier.size();

    if (size == 0)
      return "";

    if (identifier[0] == '"' || identifier[0] == '`')
      start++;

    if (identifier[size - 1] == '"' || identifier[size - 1] == '`')
      size--;

    size -= start;

    return identifier.substr(start, size);
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * @brief Remove outer quotes from any text.
   *
   * @param text Text to unquote
   * @return Return unqoted text.
   */
  std::string unquote(const std::string &text) {
    if (text.size() < 2)
      return text;

    if ((text[0] == '"' || text[0] == '`' || text[0] == '\'') && text[0] == text[text.size() - 1])
      return text.substr(1, text.size() - 2);
    return text;
  }

  //--------------------------------------------------------------------------------------------------

  std::string quote_identifier(const std::string &identifier, const char quote_char) {
    return quote_char + identifier + quote_char;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Quotes the given identifier, but only if it needs to be quoted.
   */
  std::string quoteIdentifierIfNeeded(const std::string &ident, const char quote_char, MySQLVersion version) {
    bool needs_quotation = MySQLSymbolInfo::isReservedKeyword(ident, version);
    size_t digits = 0;

    if (!needs_quotation) {
      for (std::string::const_iterator i = ident.begin(); i != ident.end(); ++i) {
        if ((*i >= 'a' && *i <= 'z') || (*i >= 'A' && *i <= 'Z') || (*i >= '0' && *i <= '9') || (*i == '_') ||
            (*i == '$') || ((unsigned char)(*i) > 0x7F)) {
          if (*i >= '0' && *i <= '9')
            digits++;

          continue;
        }
        needs_quotation = true;
        break;
      }
    }

    if (needs_quotation || digits == ident.length())
      return quote_char + ident + quote_char;
    else
      return ident;
  }

  bool is_number(const std::string &word) {
    if (word.empty())
      return false;
    size_t i = 0;
    if (word[0] == '-')
      i++;
    for (; i < word.size(); i++)
      if (!isdigit(word[i]))
        return false;
    return true;
  }

  //--------------------------------------------------------------------------------------------------

  /**
  * @brief Determine if a string is a boolean.
  *
  * @param text Text to check
  * @return Return true if given string is a boolean.
  **/
  bool isBool(const std::string &text) {
    std::string transformed;
    std::transform(text.begin(), text.end(), std::back_inserter(transformed), ::tolower);
    if (transformed.compare("true") != 0 && transformed.compare("false") != 0)
      return false;
    return true;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Function : stl_string_compare
   * Description : comparison function to be used on the sorting process
   * Return Value : following the STL requirements should return true if the
   *                first string is lower than the second
   */
  bool stl_string_compare(const std::string &first, const std::string &second, bool case_sensitive) {
    return string_compare(first, second, case_sensitive) < 0;
  }

  //--------------------------------------------------------------------------------------------------

  int string_compare(const std::string& first, const std::string& second, bool case_sensitive) {
    std::locale loc; // Use the default locale
    std::string normalized_first = first;
    std::string normalized_second = second;

    // Normalize strings (trim whitespace, etc.)
    normalized_first.erase(std::remove_if(normalized_first.begin(), normalized_first.end(), ::isspace), normalized_first.end());
    normalized_second.erase(std::remove_if(normalized_second.begin(), normalized_second.end(), ::isspace), normalized_second.end());

    if (!case_sensitive) {
      // Convert both strings to lowercase for case-insensitive comparison
      std::transform(normalized_first.begin(), normalized_first.end(), normalized_first.begin(),
        [&loc](char c) { return std::tolower(c, loc); });
      std::transform(normalized_second.begin(), normalized_second.end(), normalized_second.begin(),
        [&loc](char c) { return std::tolower(c, loc); });
    }

    // Perform lexicographical comparison
    if (normalized_first < normalized_second) {
      return -1;
    }
    else if (normalized_first > normalized_second) {
      return 1;
    }
    else {
      return 0;
    }
  }


  //--------------------------------------------------------------------------------------------------

  /**
   * Convenience function to determine if 2 strings are the same. This works also for culturally
   * equal letters (e.g. german ß and ss) and any normalization form.
   */
  bool same_string(const std::string &first, const std::string &second, bool case_sensitive) {
    return string_compare(first, second, case_sensitive) == 0;
  }

  //--------------------------------------------------------------------------------------------------

  bool contains_string(const std::string& text, const std::string& candidate, bool case_sensitive) {
    if (text.empty() || candidate.empty())
      return false;

    std::string hay_stack = text;
    std::string needle = candidate;

    if (!case_sensitive) {
      std::locale loc;
      std::transform(hay_stack.begin(), hay_stack.end(), hay_stack.begin(),
        [&loc](char c) { return std::tolower(c, loc); });
      std::transform(needle.begin(), needle.end(), needle.begin(),
        [&loc](char c) { return std::tolower(c, loc); });
    }

    // Use std::search to find the candidate in the text
    auto it = std::search(hay_stack.begin(), hay_stack.end(),
      needle.begin(), needle.end());

    return it != hay_stack.end();
  }


  //--------------------------------------------------------------------------------------------------

  EolHelpers::Eol_format EolHelpers::detect(const std::string &text) {
    std::string::size_type pos = text.find_first_of("\r\n");
    if (std::string::npos == pos)
      return default_eol_format();
    if ('\r' == text[pos])
      return ('\n' == text[pos + 1]) ? eol_crlf : eol_cr;
    else
      return eol_lf;
  }

  int EolHelpers::count_lines(const std::string &text) {
    Eol_format eol_format = detect(text);
    char eol_sym = (eol_cr == eol_format) ? '\r' : '\n';
    return (int)std::count(text.begin(), text.end(), eol_sym);
  }

  bool EolHelpers::check(const std::string &text) {
    std::string::size_type pos = text.find_first_of("\n\r");
    if (std::string::npos == pos)
      return true;
    Eol_format eol_format = detect(text);
    if (eol_lf == eol_format) {
      if (text.find("\r") != std::string::npos)
        return false;
    } else if (eol_cr == eol_format) {
      if (text.find("\n") != std::string::npos)
        return false;
    } else if (eol_crlf == eol_format) {
      do {
        if (('\n' == text[pos]) || ('\n' != text[pos + 1]))
          return false;
        ++pos;
        ++pos;
        pos = text.find_first_of("\n\r", pos);
      } while (std::string::npos != pos);
    }
    return true;
  }

  void EolHelpers::conv(const std::string &src_text, Eol_format src_eol_format, std::string &dest_text,
                        Eol_format dest_eol_format) {
    if (src_eol_format == dest_eol_format)
      throw std::logic_error("source and target line ending formats coincide, no need to convert");

    const std::string &src_eol = eol(src_eol_format);
    const std::string &dest_eol = eol(dest_eol_format);
    std::string::size_type src_eol_length = src_eol.size();

    if (dest_eol.size() != src_eol.size()) {
      dest_text.clear();
      int line_count = count_lines(src_text);
      size_t dest_size = src_text.size() + line_count * (dest_eol.size() - src_eol.size());
      dest_text.reserve(dest_size);
      std::string::size_type prev_pos = 0;
      std::string::size_type pos = 0;
      while ((pos = src_text.find(src_eol, pos)) != std::string::npos) {
        dest_text.append(src_text, prev_pos, pos - prev_pos).append(dest_eol);
        pos += src_eol_length;
        prev_pos = pos;
      }
      dest_text.append(src_text, prev_pos, std::string::npos);
    } else {
      dest_text = src_text;
      std::string::size_type pos = 0;
      while ((pos = dest_text.find(src_eol, pos)) != std::string::npos) {
        dest_text.replace(pos, src_eol_length, dest_eol);
        pos += src_eol_length;
      }
    }
  }

  void EolHelpers::fix(const std::string &src_text, std::string &dest_text, Eol_format eol_format) {
    const std::string &dest_eol = eol(eol_format);
    std::string::size_type dest_eol_length = dest_eol.size();

    dest_text.clear();
    if (eol_crlf == eol_format) {
      int cr_count = (int)std::count(src_text.begin(), src_text.end(), '\r');
      int lf_count = (int)std::count(src_text.begin(), src_text.end(), '\n');
      int crlf_count = 0;
      {
        std::string::size_type pos = 0;
        while ((pos = src_text.find(dest_eol, pos)) != std::string::npos) {
          ++crlf_count;
          pos += dest_eol_length;
        }
      }
      size_t dest_size = src_text.size() + (cr_count - crlf_count) + (lf_count - crlf_count);
      dest_text.reserve(dest_size);
    }

    std::string::size_type prev_pos = 0;
    std::string::size_type pos = 0;
    std::string crlf = "\r\n";
    while ((pos = src_text.find_first_of(crlf, pos)) != std::string::npos) {
      dest_text.append(src_text, prev_pos, pos - prev_pos).append(dest_eol);
      if (('\r' == src_text[pos]) && ('\n' == src_text[pos + 1]))
        ++pos;
      ++pos;
      prev_pos = pos;
    }
    dest_text.append(src_text, prev_pos, std::string::npos);
  }

  //--------------------------------------------------------------------------------------------------

#include <stdexcept>
#include <string>
#include <locale>
#include <codecvt>
#include <cctype>
#include <vector>

  std::string reflow_text(const std::string& text, unsigned int line_length, const std::string& left_fill,
    bool indent_first, unsigned int max_lines) {
    bool use_fill = true;
    const unsigned int minimum_text_length = 5;

    // Check if the line length complies with the minimum required
    if (line_length < minimum_text_length)
      return "";

    // Only use left_fill when it's small enough to fit in the line
    const unsigned int left_fill_length = static_cast<unsigned>(left_fill.size());

    if (left_fill_length + minimum_text_length >= line_length)
      use_fill = false;

    // Check for empty string
    if (text.empty())
      return "";

    // Validate UTF-8 string
    try {
      std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
      converter.from_bytes(text); // Throws if invalid UTF-8 is encountered
    }
    catch (const std::range_error&) {
      throw std::invalid_argument("base::reflow_text received an invalid UTF-8 string: " + text);
    }

    const std::string initial = (indent_first && use_fill) ? left_fill : "";
    const std::string new_line = use_fill ? std::string("\n") + left_fill : std::string("\n");
    std::string result = initial;

    const char* char_string = text.c_str();
    const char* iter = char_string;

    unsigned int space_position_source = 0;
    unsigned int line_char_counter = 0;
    unsigned int line_counter = 0;
    unsigned int char_count_after_space = 0;
    unsigned int text_real_length = use_fill ? line_length - left_fill_length : line_length;

    while (*iter) {
      // Get the full UTF-8 character into the result string
      unsigned char lead = static_cast<unsigned char>(*iter);
      size_t char_len = 1;

      if (lead >= 0xC0 && lead <= 0xFD) { // Multibyte UTF-8 character
        if ((lead & 0xE0) == 0xC0) char_len = 2; // 2-byte character
        else if ((lead & 0xF0) == 0xE0) char_len = 3; // 3-byte character
        else if ((lead & 0xF8) == 0xF0) char_len = 4; // 4-byte character
      }

      result.append(iter, char_len);
      line_char_counter++;
      char_count_after_space++;

      // Check for whitespace
      if (std::isspace(static_cast<unsigned char>(*iter)) && line_char_counter > left_fill_length) {
        space_position_source = static_cast<unsigned>(iter - char_string + 1);
        char_count_after_space = 0;
      }

      if (line_char_counter == text_real_length) {
        // Special case: word as big as a line
        if (char_count_after_space == text_real_length) {
          result += new_line;
          space_position_source += char_count_after_space;
          line_char_counter = char_count_after_space = 0;
        }
        else {
          // Find last space character position in the result string
          unsigned int break_position =
            space_position_source + line_counter * static_cast<unsigned>(new_line.size()) +
            static_cast<unsigned>(initial.size());

          // Insert a newline at the right position
          result.size() == break_position ? result += new_line : result.insert(break_position, new_line);

          // Mark the characters that were already inserted after the newline
          line_char_counter = char_count_after_space;
        }

        if (++line_counter == max_lines) {
          result.resize(result.size() - char_count_after_space - new_line.size());
          result += "\n(...)";
          break;
        }
      }

      iter += char_len; // Move to the next character
    }

    return result;
  }


} // namespace base
