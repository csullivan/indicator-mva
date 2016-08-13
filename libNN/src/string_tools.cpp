
/* 
 * M M Reid - 2013. implementation of useful string manipulation tools
*/

#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>

#include "string_tools.hpp"

//**********************************************************************************
void string_tools::replaceAll(std::string& str, const std::string& from, const std::string& to) 
{
  if(from.empty())
    return;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

//**********************************************************************************
std::string string_tools::removeBadChars(const std::string& input) const
{
  std::string output;
  //decode first bad char's
  size_t firstOcc = input.find_first_of("&|}{()][#><%\"\'");
  output.append(input,0,firstOcc);
  // append the good characters that survive.
  if ( firstOcc!=std::string::npos) {
    std::string residual = input.substr(firstOcc+1);
    std::string add_output = removeBadChars(residual);
    output.append(add_output);
  }
  return output;
}

//**********************************************************************************
/*
 * contains: checks that string contains some substring, or is matched
*/
bool string_tools::contains(const std::string& str, const std::string& substr) const
{
   if ( str == substr ) return true;
   if ( str.find( substr ) != std::string::npos ) return true;
   else return false;
}

//**********************************************************************************
/*
 * is_digit: checks that string contains digits
*/
bool string_tools::is_digit(const std::string& s) const
{
   size_t len = s.size();
   if (len == 0) return false;
   int b (0), d (0);
   for (size_t i (0); i < len; ++i) {
      if ( s[i] == '.' ) continue;
      if (s[i] != ' ' && !isdigit(s[i])) return false;
      if (s[i] == ' ') b++;
      if (isdigit(s[i])) d++;
   }
   if (b && !d)
      return false;
   return true;
}

//**********************************************************************************
/*
 * lstrip: Return a copy of s with whitespace removed from left
 */
std::string& string_tools::lstrip(std::string& s) 
{
    std::string::size_type p = s.find_first_not_of(" \n\r\t");
    s.erase(0, p);
    return s;

}

//**********************************************************************************
/*
 * rstrip: Return a copy of s with whitespace removed from right
 */
std::string& string_tools::rstrip(std::string& s) 
{
    std::string::size_type p = s.find_last_not_of(" \n\r\t");
    s.erase(p + 1);
    return s;
}

//**********************************************************************************
/*
 * strip: Return a copy of s with leading and trailing whitespace
 *   removed
 */
std::string& string_tools::removeWhiteSp(std::string& s) 
{
    return rstrip(lstrip(s));
}
//**********************************************************************************
/*
 * is_alpha: returns true if all chars in string are alphabetic
*/
bool string_tools::is_alpha(const std::string& s) const
{
   size_t len = s.length();
   if (len == 0) return false;
   for (size_t i = 0; i < len; ++i)
      if (!isalpha(s[i]))
         return false;
   return true;
}


//**********************************************************************************
/*
 * invalidChar: returns true if char is ascii defined.
*/
bool invalidChar (const char& c) {

    return !( c>= 0 && c < 128 );   
} 

//**********************************************************************************
/*
 * stripUnicode: removes any non ascii characters from the string
*/
void string_tools::stripUnicode(std::string & str) { 

    str.erase( std::remove_if( str.begin(), str.end(), invalidChar), str.end() );  
}

//**********************************************************************************
/*
 * is_alpha: returns true if all chars in string are alphabetic
*/
bool string_tools::is_ascii(const std::string& str) const{
    size_t len = str.length();
    for(size_t i(0); i < len; ++i) {
        if (str[i] & ~0x7F) {
            return false;
        }
    }
    return true;
}
//**********************************************************************************
/*
 * to_upper: convert all lowercase characters to uppercase
*/
std::string string_tools::to_upper(const std::string& s) const
{
    std::string temp;
    std::transform(s.begin(), s.end(), std::back_inserter(temp), toupper);
    return temp;
  std::string output;
}

//**********************************************************************************
/*
 * to_lower: convert all uppercase characters to lowercase
*/
std::string string_tools::to_lower(const std::string& s) const
{
    std::string temp;
    std::transform(s.begin(), s.end(), std::back_inserter(temp), tolower);
    return temp;
}

//**********************************************************************************
void string_tools::replaceAfter(std::string& str, const std::string& text) 
{
  if(text.empty())
    return ;
  size_t start_pos = 0;
  while((start_pos = str.find(text, start_pos)) != std::string::npos) {
    str.erase(start_pos);
  }
}

//**********************************************************************************
void string_tools::replaceBefore(std::string& str, const std::string&text) 
{
  if(text.empty())
    return ;
  size_t start_pos = 0;
  while((start_pos = str.find(text, start_pos)) != std::string::npos) {
    str.erase(0,start_pos+text.length());
  }
}

//**********************************************************************************
/*
 * split: Return a vector of strings by splitting str at each occurrence
 * of the separator character.
 */
std::vector<std::string> string_tools::split(const std::string& str, const std::string& separator)
{
    std::vector<std::string> t;
    Splitter tok(str, separator);
    while (tok.good())
        t.push_back(tok.next());
    return t;
}
