#ifndef __AFEB_teststand_utils_IO_h__
#define __AFEB_teststand_utils_IO_h__

#include <vector>
#include <map>
#include <iostream>

using namespace std;

namespace AFEB { namespace teststand {

    template <typename T>
    ostream& operator<<( ostream& os, const vector<T>& t ){
      cout << "[";
      typename vector<T>::const_iterator it;
      for ( it = t.begin(); it != t.end(); ++it ) cout << *it << (it+1 != t.end() ? "," : "");
      cout << "]";
      return os;
    }
    
    template <typename T1, typename T2>
    ostream& operator<<( ostream& os, const pair<T1,T2>& t ){
      cout << "(" << t.first << "|" << t.second << ")";
      return os;
    }
    
    template <typename T1, typename T2>
    ostream& operator<<( ostream& os, const map<T1,T2>& t ){
      cout << "(";
      typename map<T1,T2>::const_iterator it, iti;
      for ( it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti )
	cout << it->first << "->" << it->second << (iti != t.end() ? "," : "");
      cout << ")";
      return os;
    }
    
  }}

#endif
