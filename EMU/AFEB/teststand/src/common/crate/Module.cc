#include "AFEB/teststand/crate/Module.h"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace AFEB::teststand;

ostream& AFEB::teststand::operator<<( ostream& os, const Module& m ){
  os << "Module name: \t" << setw(20) << m.getName()
     << "\t id: \t"       << m.getId()
     << "\t type: \t"     << setw(20) << m.getType()
     << "\t slot: \t"     << m.getSlot()
     << endl;
  return os;
}
