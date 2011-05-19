#ifndef __AFEB_teststand_Crate_h__
#define __AFEB_teststand_Crate_h__

// #include "AFEB/teststand/Module.h"
#include "AFEB/teststand/CrateController.h"

#include <string>
#include <vector>
#include <ostream>

using namespace std;

namespace AFEB { namespace teststand {

    ostream& operator<<( ostream& os, const AFEB::teststand::Crate& c );

    class Module;
    class CrateController;

    class Crate{
    public:
      enum { maxModules_ = 24 };

      friend ostream& operator<<( ostream& os, const AFEB::teststand::Crate& c );

      Crate();
      ~Crate();
      int getNumber() const;
      const AFEB::teststand::Module* getModule( const int slot ) const { return modules_[slot]; }
      const AFEB::teststand::CrateController* getCrateController() const { return static_cast<const AFEB::teststand::CrateController*> ( modules_[controllerSlot_] ); }
      void insert( AFEB::teststand::Module* module, int slot );
      void insertController( AFEB::teststand::CrateController* controller, int slot );
    private:
      string name_;
      string type_;
      vector<AFEB::teststand::Module*> modules_;
      int    controllerSlot_;
    };

  }}

#endif
