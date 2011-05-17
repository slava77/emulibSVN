#ifndef __AFEB_teststand_Module_h__
#define __AFEB_teststand_Module_h__

//#include "AFEB/teststand/Crate.h"

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Crate;

    class Module{
    public:
      Module( const string name, const string type )
	: name_( name ),
	  type_( type ){}
      virtual ~Module(){}
      string getName() const { return name_; }
      string getType() const { return type_; }
      const AFEB::teststand::Crate* getCrate() const { return crate_; }
      int getSlot() const { return slot_; }
      void setSlot( const int slot ){ slot_ = slot; }
    protected:
      string name_;
      string type_;
      AFEB::teststand::Crate* crate_;
      int    slot_;
    };

  }}

#endif
