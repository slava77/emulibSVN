#ifndef __emu_daq_manager_FactTypes_h__
#define __emu_daq_manager_FactTypes_h__

namespace emu { namespace daq { namespace manager {

  class LocalDAQStatusFact {
  public:
    enum ParameterName_t { runType, state, daqState, isSupervised, isBuildingEvents, nParameters };
    static const char*  getTypeName() { return typeName_; }
    static const string getParameterName( const ParameterName_t p ) { return parameterNames_[p]; }
  protected:
    static const char* const typeName_; 
    static const char* const parameterNames_[nParameters];
  };
  const char* const LocalDAQStatusFact::typeName_ = "LocalDAQStatusFact";
  const char* const LocalDAQStatusFact::parameterNames_[] = { "runType", "state", "daqState", "isSupervised", "isBuildingEvents" };

}}} // namespace emu::daq::manager

#endif
