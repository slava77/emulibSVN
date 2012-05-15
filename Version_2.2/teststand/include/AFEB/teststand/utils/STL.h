#ifndef __AFEB_teststand_utils_STL_h__
#define __AFEB_teststand_utils_STL_h__

#include <set>
#include <map>
#include <string>
#include <valarray>
#include <vector>

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

      /// Transfer data between STL containers. The target container's data should be accessible by operator[].
      /// This works fine with g++ v4.1.2, but fails to compile with v4.5.3 and v4.6.1.
      ///
      /// @param s Source container.
      ///
      /// @return Target container.
      ///
      template <class T, template<typename> class Source, template<typename> class Target>
      Target<T> transfer( const Source<T>& s ){
	Target<T> t( s.size() );
	int i=0;
	for ( typename Source<T>::const_iterator is=s.begin(); is!=s.end(); ++is ) t[i++] = *is;
	return t;
      }

      /// Transfer data from STL vector to valarray.
      ///
      /// @param v Source vector.
      ///
      /// @return Target valarray.
      ///
      template <typename T>
      valarray<T> valarrayFromVector( const vector<T>& v ){
	valarray<T> a( v.size() );
	int i=0;
	for ( typename vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi ) a[i++] = *vi;
	return a;
      }

    }
  }
}

#endif
