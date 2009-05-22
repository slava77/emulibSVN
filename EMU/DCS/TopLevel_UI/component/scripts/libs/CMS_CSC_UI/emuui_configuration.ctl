/**@file

This package contains functions to get configuration parameters and mappings.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

global mapping emuui_g_mappingCache;

/** @return a mapping of a given name (the mapping is retrieved from configuration DPs). */
mapping emuui_getMapping(string name, dyn_string &exceptionInfo, string version = "") {
  if (strlen(version) > 0) { version = "_" + version; } // add prefix to version if it's provided
  
  //check the cache
  if (mappingHasKey(emuui_g_mappingCache, name + version)) {
    return emuui_g_mappingCache[name + version];
  }
  
  string dpName = "emuui_map_" + name + version;
  if (!dpExists(dpName)) {
    emu_addError("Requested mapping \"" + name + version + "\" does not exist", exceptionInfo);
    return emucdb_dummyMapping;
  }

  dyn_string data;
  dpGet(dpName + ".map", data);
  
  mapping ret;
  dyn_string tmpSplit;
  for (int i=1; i <= dynlen(data); i++) {
    tmpSplit = strsplit(data[i], ";");
    if (dynlen(tmpSplit) != 2) {
      emu_addError("Lines #" + i + " in mapping \"" + name + version + "\" is corrupted", exceptionInfo);
      return emucdb_dummyMapping;
    }
    if (mappingHasKey(ret, tmpSplit[1])) {
      emu_addError("Key \"" + tmpSplit[1] + "\" in mapping \"" + name + version + "\" is defined multiple times", exceptionInfo);
      return emucdb_dummyMapping;
    }
    
    ret[tmpSplit[1]] = tmpSplit[2];
  }
  //add the mapping to the cache
  emuui_g_mappingCache[name + version] = ret;
      
  return ret;
}

/** @return a value of a requested key in a given mapping. */
anytype emuui_getMappingValue(string name, string key, dyn_string &exceptionInfo, string version = "") {
  mapping map = emuui_getMapping(name, exceptionInfo, version);
  if (emu_checkException(exceptionInfo)) { return; }
  if (!mappingHasKey(map, key)) {
    emu_addError("Key \"" + key + "\" does not exist in mapping \"" + name + version + "\"", exceptionInfo);
    return "";
  }
  return map[key];
}

/** Checks if a key exists in a given mapping (if the mapping itself exists).
    Doesn't generate any exceptions - all are handled here. If indeed the key is reachable - returns true.
*/
bool emuui_mappingHasKey(string mappingName, string key, string mappingVersion = "") {
  dyn_string ex;
  emuui_getMappingValue(mappingName, key, ex, version);
  if (dynlen(ex) > 0) {
    emu_errorHandled(ex);
    return false;
  }
  return true;
}

/** Clears the mapping cache. */
void emuui_clearMappingCache() {
  mappingClear(emuui_g_mappingCache);
}

/**
    @param parameters    describes what FSM node you are interested in e.g. for chamber you give side, station, ring and chamberNumber.
    @return an FSM node of a requested type and matching the given parameters.
*/
string emuui_getFsmNode(string type, mapping parameters, dyn_string &exceptionInfo) {
  string pattern = emuui_getMappingValue("fsmNodePatterns", type, exceptionInfo);
  if (dynlen(exceptionInfo) > 0) {
    emu_errorHandled(exceptionInfo);
    return "";
  }

  return _emuui_fillPattern(pattern, parameters);  
}

/**
    @param type          what type of DP are you interested in? e.g. temperature_max
    @param parameters    describes what DP you are interested in e.g. for chamber you give side, station, ring and chamberNumber.
    @return              name of a DP that matches the requested type and given parameters.
*/
string emuui_getDpName(string type, mapping parameters, dyn_string &exceptionInfo) {
  dyn_string dps = emuui_getDpNames(type, parameters, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; } // should never happen as they're handled in emuui_getDpNames(...)
  
  if (dynlen(dps) < 1) {
    return "";
  }
  
  if (dynlen(dps) > 1) {
    emu_info("WARNING: more than one match found for DP request '" + dp + "'. Returned the first one");
  }
  
  return dps[1];
}

/**
    @param type          what type of DPs are you interested in? e.g. HV_vMon, HV_Primary_vMon, HV_Primary_iMon...
    @param parameters    describes what DPs you are interested in e.g. for chamber you give side, station, ring and chamberNumber.
    @return              an array of DP names that match the requested type and given parameters.
*/
dyn_string emuui_getDpNames(string type, mapping parameters, dyn_string &exceptionInfo) {
  dyn_string ret;
  
  string pattern = emuui_getMappingValue("dpNamePatterns", type, exceptionInfo);
  if (dynlen(exceptionInfo) > 0) {
    emu_errorHandled(exceptionInfo);  
    return ret;
  }

  string dp = _emuui_fillPattern(pattern, parameters);
  ret = dpNames("*:" + dp);
  
  return ret;
}

/** Substitutes parameter values in the pattern. Parameters in patterns have dollar symbols on both sides of the parameter name.
  e.g. pattern "LowVoltage/CSC_ME_$side$$station$$ring$_C$chamberNumber$_LV" */

string _emuui_fillPattern(string pattern, mapping parameters) {
  for(int i=1; i <= mappinglen(parameters); i++) {
    string key = mappingGetKey(parameters, i);
    string value = parameters[key];
    
    strreplace(pattern, "$" + key + "$", value);
  }
  return pattern;
}
