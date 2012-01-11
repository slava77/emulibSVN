#ifndef __AFEB_teststand_Module_h__
#define __AFEB_teststand_Module_h__

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Crate;

    class Module{
    public:
      friend ostream& operator<<( ostream& os, const Module& m );
      enum Type_t { CrateController, PulseGenerator, SignalConverter, TDC, nTypes };
      Module( const string name, const AFEB::teststand::Module::Type_t type, const string id );
      Module( const string name, const string type, const string id );
      virtual ~Module(){}
      string getName() const { return name_; }
      Type_t getType() const { return type_; }
      string getTypeString() const { return typeStrings_[type_]; }
      string getId() const { return id_; }
      const AFEB::teststand::Crate* getCrate() const { return crate_; }
      int getSlot() const { return slot_; }
      void setCrate( AFEB::teststand::Crate* crate ){ crate_= crate; }
      void setSlot( const int slot ){ slot_ = slot; }
      static Type_t getType      ( const string& typeString );
      static string getTypeString( const Type_t  type       );      
    protected:
      string name_;
      Type_t type_;
      string id_;
      AFEB::teststand::Crate* crate_;
      int    slot_;
      static const char* const typeStrings_[nTypes];
    };
    
    ostream& operator<<( ostream& os, const Module& m );

  }}

#endif
