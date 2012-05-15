#ifndef __AFEB_teststand_utils_String_h__
#define __AFEB_teststand_utils_String_h__

#include <string>
#include <sstream>
#include <vector>

namespace AFEB { namespace teststand { namespace utils {

      template <typename T>
      T stringTo( const std::string s ){
	std::stringstream ss( s );
	T t;
	ss >> t;
	return t;
      }
      
      template <typename T>
      std::string stringFrom( const T& t ){
	std::stringstream ss;
	ss << t;
	return ss.str();
      }
      
      template <typename T>
      std::string stringFrom( const T* t ){
	std::stringstream ss;
	ss << *t;
	return ss.str();
      }

      std::vector<std::string> splitSting( const std::string& str, const std::string& delimiter );

      std::string shaveOffBlanks( const std::string& str );

  }}}

#endif
