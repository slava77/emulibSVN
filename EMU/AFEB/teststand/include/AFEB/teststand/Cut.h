#ifndef __AFEB_teststand_Cut_h__
#define __AFEB_teststand_Cut_h__

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand {

    class Cut;
    ostream& operator<<( ostream& os, const Cut& c );

    class Cut{
    public:
      friend ostream& operator<<( ostream& os, const Cut& c );
      Cut( const vector< pair<string,string> >& parameters );
      bool accepts( const double value ) const { return ( ( low_ < value ) && ( value < high_ ) ); }
      string getId() const { return id_; }
      string getXpath() const { return xpath_; }

    private:
      string id_;
      double low_;
      double high_;
      string description_;
      string xpath_;		///< XPath to value in results' XML file
    };

  }
}

#endif
