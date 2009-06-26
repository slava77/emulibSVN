/**@file

This package contains functions to get configuration parameters and mappings.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

global mapping emuui_g_mappingCache;
global mapping emuui_g_arrayCache;
global dyn_string emuui_g_lvSystemNames;
const string emuui_g_version = ""; // default version is empty, if any other version is specified then you have to make sure that you have all the datapoints available i.e. CSC_UI_mapping and CSC_UI_array with appendix "_<version>"

/** @return a mapping of a given name (the mapping is retrieved from configuration DPs). */
mapping emuui_getMapping(string name, dyn_string &exceptionInfo) {
  string version = emuui_g_version;
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
  
  mapping ret = _emuui_constructMappingFromCSV(data, exceptionInfo, name, version);
  if (emu_checkException(exceptionInfo)) { return emucdb_dummyMapping; }

  //add the mapping to the cache
  emuui_g_mappingCache[name + version] = ret;
      
  return ret;
}

/** Converts an array of comma separated values to mapping. 
  Parameters name and version are optional and only used for additional information in error messages.
  If ingnoreEmptyValue is true then no error message is produced when the value is empty (not recommended, but needed sometimes).
*/
mapping _emuui_constructMappingFromCSV(dyn_string csvArray, dyn_string exceptionInfo, string name="", string version="", bool tolerateEmptyValue = false) {
  mapping ret;
  dyn_string tmpSplit;
  for (int i=1; i <= dynlen(csvArray); i++) {
    tmpSplit = strsplit(csvArray[i], ";");
    if (dynlen(tmpSplit) != 2) {
      bool tolerate = false;
      if (tolerateEmptyValue) {
        if ((dynlen(tmpSplit) == 1) && (strpos(csvArray[i], ";") > 0)) { // only value is empty
          dynAppend(tmpSplit, "");
          tolerate = true;
        }
      }
      if (!tolerate) {
        emu_addError("Line #" + i + " in mapping \"" + name + version + "\" is corrupted", exceptionInfo);
        return emucdb_dummyMapping;
      }
    }
    if (mappingHasKey(ret, tmpSplit[1])) {
      emu_addError("Key \"" + tmpSplit[1] + "\" in mapping \"" + name + version + "\" is defined multiple times", exceptionInfo);
      return emucdb_dummyMapping;
    }
    
    ret[tmpSplit[1]] = tmpSplit[2];
  }
  return ret;
}

/** @return a value of a requested key in a given mapping. */
anytype emuui_getMappingValue(string name, string key, dyn_string &exceptionInfo) {
  mapping map = emuui_getMapping(name, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  if (!mappingHasKey(map, key)) {
    emu_addError("Key \"" + key + "\" does not exist in mapping \"" + name + "\"", exceptionInfo);
    return "";
  }
  return map[key];
}

/** Checks if a key exists in a given mapping (if the mapping itself exists).
    Doesn't generate any exceptions - all are handled here. If indeed the key is reachable - returns true.
*/
bool emuui_mappingHasKey(string mappingName, string key) {
  dyn_string ex;
  emuui_getMappingValue(mappingName, key, ex);
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

  return emuui_fillPattern(pattern, parameters);  
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
    emu_info("WARNING: more than one match found  (" + dynlen(dps) + ") for DP request of type '" + type + "'. Returned the first one");
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

  string dp = emuui_fillPattern(pattern, parameters);
  ret = dpNames("*:" + dp);
  
  return ret;
}

/** read the emuui_map_dpValueRanges map and return the minimum value for a given type. Exception is thrown if mapping or type does not exist. */
float emuui_getDpValueRangeMin(string type, dyn_string &exceptionInfo) {
  float ret = emuui_getMappingValue("dpValueRanges", "min_" + type, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  return ret;
}

/** read the emuui_map_dpValueRanges map and return the maximum value for a given type. Exception is thrown if mapping or type does not exist. */
float emuui_getDpValueRangeMax(string type, dyn_string &exceptionInfo) {
  float ret = emuui_getMappingValue("dpValueRanges", "max_" + type, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  return ret;
}

/** Substitutes parameter values in the pattern. Parameters in patterns have dollar symbols on both sides of the parameter name.
  e.g. pattern "LowVoltage/CSC_ME_$side$$station$$ring$_C$chamberNumber$_LV".
  Note any remaining $ parameters which were not found in the parameters mapping are replaced by a wildcard - "*".
*/
string emuui_fillPattern(string pattern, mapping parameters) {
  for(int i=1; i <= mappinglen(parameters); i++) {
    string key = mappingGetKey(parameters, i);
    string value = parameters[key];
    
    strreplace(pattern, "$" + key + "$", value);
  }
  while (patternMatch("*$*$*", pattern)) {
    dyn_string split = strsplit(pattern, "$");
    string param = "$" + split[2] + "$";
    strreplace(pattern, param, "*");
  }
  return pattern;
}

dyn_string emuui_getArray(string name, dyn_string &exceptionInfo) {
  string version = emuui_g_version;
  if (strlen(version) > 0) { version = "_" + version; } // add prefix to version if it's provided
  
  //check the cache
  if (mappingHasKey(emuui_g_arrayCache, name + version)) {
    return emuui_g_arrayCache[name + version];
  }
  
  string dpName = "emuui_array_" + name + version;
  if (!dpExists(dpName)) {
    emu_addError("Requested array \"" + name + version + "\" does not exist", exceptionInfo);
    return makeDynString();
  }

  dyn_string data;
  dpGet(dpName + ".array", data);
  
  emuui_g_arrayCache[name + version] = data;
      
  return data;
}

/** returns names of the Low Voltage systems that are now reachable. */
dyn_string emuui_getLvSystemNames(dyn_string &exceptionInfo) {
  if (dynlen(emuui_g_lvSystemNames) > 0) {
    return emuui_g_lvSystemNames;
  }
  
  //get systems list, pick those with "LV" and query them for PCMB DB
  dyn_string sysNames;
  dyn_uint sysIds;
  getSystemNames(sysNames, sysIds);
  for (int i=1; i <= dynlen(sysNames); i++) {
    if (strpos(strtolower(sysNames[i]), "lv") >= 0) { // contains LV (case insensitive).
      dynAppend(emuui_g_lvSystemNames, sysNames[i]);
    }
  }
  if (dynlen(emuui_g_lvSystemNames) == 0) {
    emu_addError("No Low Voltage systems found (Low Voltage system name should contain 'lv' in the name (case insensitive))", exceptionInfo);
    return emuui_g_lvSystemNames;
  }
  
  return emuui_g_lvSystemNames;
}
