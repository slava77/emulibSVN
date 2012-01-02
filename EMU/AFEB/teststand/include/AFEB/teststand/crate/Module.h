#ifndef __AFEB_teststand_Module_h__
#define __AFEB_teststand_Module_h__

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Crate;

    class Module{
    public:
      friend ostream& operator<<( ostream& os, const Module& m );
      Module( const string name, const string type, const string id )
	: name_( name ),
	  type_( type ),
	  id_  (   id ){}
      virtual ~Module(){}
      string getName() const { return name_; }
      string getType() const { return type_; }
      string getId() const { return id_; }
      const AFEB::teststand::Crate* getCrate() const { return crate_; }
      int getSlot() const { return slot_; }
      void setCrate( AFEB::teststand::Crate* crate ){ crate_= crate; }
      void setSlot( const int slot ){ slot_ = slot; }
    protected:
      string name_;
      string type_;
      string id_;
      AFEB::teststand::Crate* crate_;
      int    slot_;
    };
    
    ostream& operator<<( ostream& os, const Module& m );

  }}

#endif
