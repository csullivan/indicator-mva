#ifndef STRING_TOOLS_HPP
#define STRING_TOOLS_HPP 1

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/*
   	M M Reid - 22.01.2012
	string_tools class: Aids user to find and replace parts or all of a string
	given some initial consideration; before, after or all instances.
	Also removes bad characters.
*/

class string_tools {

 public:
  
  string_tools() {};
  ~string_tools() {};
  
  void replaceAll(std::string& str, const std::string& from, const std::string& to);
  void replaceAfter(std::string& str, const std::string& text);
  void replaceBefore(std::string& str, const std::string& text);

  std::string removeBadChars(const std::string& input) const;
  // lowercase characters to uppercase
  std::string to_upper(const std::string& input) const;
  // uppercase characters to lowercase
  std::string to_lower(const std::string& input) const;
  // whitespace removed from left
  std::string& lstrip(std::string& s) ;
  // whitespace removed from right
  std::string& rstrip(std::string& s) ;
  // trailing and leading whitespace removed
  std::string& removeWhiteSp(std::string& s);
  // returns vector of strings split by some delimiter, ',' for example
  std::vector<std::string> split(const std::string& str, const std::string& separator);
  
  // checks if string is made of digits
  bool is_digit(const std::string& s) const;
  // checks if chars in string are alphabetic
  bool is_alpha(const std::string& s) const;
  // checks if string (str) contrains a sub string (substr)
  bool contains(const std::string& str, const std::string& substr) const;
  // checks that string values are ascii defined
  bool is_ascii(const std::string& str) const;
  // check if char is ascii
  friend bool invalidChar(const char& c) ;
  // removes non-ascii characters from string
  void stripUnicode(std::string & str);
    
  // Convert type to string, no saftey check though
  template <class T>
  inline std::string to_string (const T& t) {
    std::ostringstream ss;
    ss << t;
    return ss.str();
  };

  // Convert string to type, again no saftey catch.
  template <class T>
  inline T to_number (const std::string& s) {
    std::istringstream i(s);
    double x(0.0);
    if (!(i >> x))
      return 0;
    return x;
  }; 

};

/*
 * class Splitter: Splits strings into parts based on
 *   a separator character.
 */
class Splitter {
    public:
        // Constructor: str is the string to be tokenized, separator
        // is a single character. At each occurrence of separator,
        // the string will be split.
        Splitter(const std::string& str, const std::string& separator )
            : s(str), sep(separator), p(0), q(0) {}
        
        // next(): Return the next token
        std::string next()
        {
            q = s.find(sep, p);
            std::string tmp(s, p, q - p);
            p = q + 1;
            if (q == std::string::npos)
                p = q;
            return tmp;
        }

        // good(): indicate that the splitter state is good (i.e.
        // the end of the string has not been reached)
        bool good() { return p != std::string::npos; }

    private:
        const std::string& s; // ref. to the string being tokenized
        const std::string& sep; // separator character
        std::string::size_type p; // p, q: tokenizer state
        std::string::size_type q; // as positions in the string
}; // class Splitter




#endif
