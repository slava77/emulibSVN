#ifndef __AFEB_teststand_Results_h__
#define __AFEB_teststand_Results_h__

#include "toolbox/BSem.h"

#include "TH2D.h"
#include "TTree.h"

#include "AFEB/teststand/Measurement.h"

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Measurement;
    class TestedDevice;

    class Results{
    public:
      Results( const Measurement* const measurement, const TestedDevice* const device );
      void add( int const amplitude, const vector<double>& times );
    private:
      toolbox::BSem bsem_;
      const Measurement* const measurement_;
      const TestedDevice* const testedDevice_;
      TTree *times_;
      TH2D *pulse_;
    };

  }}

#endif
