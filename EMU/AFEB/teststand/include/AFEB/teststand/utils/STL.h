#ifndef __AFEB_teststand_utils_IO_h__
#define __AFEB_teststand_utils_STL_h__

#include <set>
#include <map>

using namespace std;

namespace AFEB { namespace teststand { namespace utils {

      template <typename Key, typename T>
      const set<Key> keys( const map<Key,T> m ){
	set<Key> s;
	for ( typename map<Key,T>::const_iterator i=m.begin(); i!=m.end(); ++i ) s.insert( i->first );
	return s;
      }

    }
  }
}

#endif
