#ifndef __AFEB_teststand_Crate_h__
#define __AFEB_teststand_Crate_h__

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand {

    class Crate{
    public:
      Crate( const int number )
	: number_( number ){}
      string getName() const { return name_; }
      string getType() const { return type_; }
      int getNumber() const { return number_; }
      const AFEB::teststand::Module* getModule( const int slot ) const { return modules_[slot]; }
      const AFEB::teststand::Module* getCrateController() const { return modules_[controllerSlot_]; }
      void insert( AFEB::teststand::Module* module, int slot );
      void insertController( AFEB::teststand::Module* module, int slot );
    private:
      string name_;
      string type_;
      string number_;
      vector<AFEB::teststand::Module*> modules_;
      int controllerSlot_;
    };

  }}

#endif
