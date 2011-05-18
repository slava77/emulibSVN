#ifndef __AFEB_teststand_utils_String_h__
#define __AFEB_teststand_utils_String_h__

#include <string>
#include <sstream>

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

  }}}

#endif
