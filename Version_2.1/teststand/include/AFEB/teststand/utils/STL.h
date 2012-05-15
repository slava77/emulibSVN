#ifndef __AFEB_teststand_utils_IO_h__
#define __AFEB_teststand_utils_STL_h__

#include <set>
#include <map>
#include <string>
#include <valarray>

using namespace std;

namespace AFEB { namespace teststand { namespace utils {

      template <typename Key, typename T>
      const set<Key> keys( const map<Key,T> m ){
	set<Key> s;
	for ( typename map<Key,T>::const_iterator i=m.begin(); i!=m.end(); ++i ) s.insert( i->first );
	return s;
      }

      template <typename T>
      map<string,double> statistics( const valarray<T>& a ){
	map<string,double> stats;
	double mean = a.sum() / a.size();
	stats["mean"  ] = mean;
	//cout << valarray<double>( pow( a - mean, 2 ) ) << endl;
	stats["rms"        ] = pow( ( pow( a - mean, 2 ) ).sum() / a.size(), 0.5 );
	stats["min"        ] = a.min();
	stats["max"        ] = a.max();
	stats["maxAbsResid"] = max( a.max() - mean, mean - a.min() );
	return stats;
      }

    }
  }
}

#endif
