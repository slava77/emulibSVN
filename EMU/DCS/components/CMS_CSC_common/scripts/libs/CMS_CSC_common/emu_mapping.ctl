// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"
#uses "CMS_CSC_common/emu_util.ctl"

/**@file

This package contains functions to lookup various mappings (stored in EMU_mapping datapoints) and configuration parameters.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

global mapping emu_g_mappingCache;
global mapping emu_g_arrayCache;
global string emu_g_cscSystemNamesPattern;

/** @return a mapping of a given name (the mapping is retrieved from configuration DPs). */
mapping emu_getMapping(string name, dyn_string &exceptionInfo) {
  //check the cache
  if (mappingHasKey(emu_g_mappingCache, name)) {
    return emu_g_mappingCache[name];
  }
  
  dyn_string data = emu_getMappingAsCSV(name, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return EMU_DUMMY_MAPPING; }
  
  mapping ret = _emu_constructMappingFromCSV(data, exceptionInfo, name);
  if (emu_checkException(exceptionInfo)) { return EMU_DUMMY_MAPPING; }

  //add the mapping to the cache
  emu_g_mappingCache[name] = ret;
      
  return ret;
}

/** @return a mapping of a given name in CSV array (the mapping is retrieved from configuration DPs). */
dyn_string emu_getMappingAsCSV(string name, dyn_string &exceptionInfo) {
  string dpName = "emu_map_" + name;
  if (!dpExists(dpName)) {
    emu_addError("Requested mapping \"" + name + "\" does not exist", exceptionInfo);
    return makeDynString();
  }

  dyn_string data;
  dpGet(dpName + ".map", data);
  
  return data;
}

/** Converts an array of comma separated values to mapping. 
  Parameter name is optional and only used for additional information in error messages.
  If tolerateEmptyValue is true then no error message is produced when the value is empty (not recommended, but needed sometimes).
*/
mapping _emu_constructMappingFromCSV(dyn_string csvArray, dyn_string &exceptionInfo, string name="", bool tolerateEmptyValue = false) {
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
        emu_addError("Line #" + i + " in mapping \"" + name + "\" is corrupted", exceptionInfo);
        return EMU_DUMMY_MAPPING;
      }
    }
    if (mappingHasKey(ret, tmpSplit[1])) {
      emu_addError("Key \"" + tmpSplit[1] + "\" in mapping \"" + name + "\" is defined multiple times", exceptionInfo);
      return EMU_DUMMY_MAPPING;
    }
    
    ret[tmpSplit[1]] = tmpSplit[2];
  }
  return ret;
}

/** Converts an array of strings to one-to-many mapping (key -> array[value1, value2...]. 
  Parameter name is optional and only used for additional information in error messages.
  Note the format of the strings must be the following: key;value1,value2,...,valueN
  If tolerateEmptyValue is true then no error message is produced when the value is empty (not recommended, but needed sometimes).
*/
mapping _emu_constructOneToManyMapping(dyn_string source, dyn_string &exceptionInfo, string name="", bool tolerateEmptyValue = false) {
  mapping ret;
  dyn_string splitKeyValues;
  for (int i=1; i <= dynlen(source); i++) {
    splitKeyValues = strsplit(source[i], ";");
    if (dynlen(splitKeyValues) != 2) {
      bool tolerate = false;
      if (tolerateEmptyValue) {
        if ((dynlen(splitKeyValues) == 1) && (strpos(source[i], ";") > 0)) { // only value is empty
          dynAppend(splitKeyValues, "");
          tolerate = true;
        }
      }
      if (!tolerate) {
        emu_addError("Line #" + i + " in mapping \"" + name + "\" is corrupted", exceptionInfo);
        return EMU_DUMMY_MAPPING;
      }
    }
    if (mappingHasKey(ret, splitKeyValues[1])) {
      emu_addError("Key \"" + splitKeyValues[1] + "\" in mapping \"" + name + "\" is defined multiple times", exceptionInfo);
      return EMU_DUMMY_MAPPING;
    }
    
    ret[splitKeyValues[1]] = strsplit(splitKeyValues[2], ",");
  }
  return ret;
}

/** @return a value of a requested key in a given mapping. */
anytype emu_getMappingValue(string name, string key, dyn_string &exceptionInfo) {
  mapping map = emu_getMapping(name, exceptionInfo);
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
bool emu_mappingHasKey(string mappingName, string key) {
  dyn_string ex;
  mapping map = emu_getMapping(mappingName, ex);
  if (dynlen(ex) > 0) {
    emu_errorHandled(ex);
    return false;
  }
  
  return mappingHasKey(map, key);
}

/** Clears the mapping cache. */
void emu_clearMappingCache() {
  mappingClear(emu_g_mappingCache);
}

/**
    @param type          what type of DP are you interested in? e.g. temperature_max
    @param parameters    describes what DP you are interested in e.g. for chamber you give side, station, ring and chamberNumber.
    @return              name of a DP that matches the requested type and given parameters.
*/
string emu_getDpName(string type, mapping parameters, dyn_string &exceptionInfo, bool localSystemOnly = true) {
  dyn_string dps = emu_getDpNames(type, parameters, exceptionInfo, localSystemOnly);
  if (emu_checkException(exceptionInfo)) { return; } // should never happen as they're handled in emu_getDpNames(...)
  
  if (dynlen(dps) < 1) {
    return "";
  }
  
  if (dynlen(dps) > 1) {
    emu_info("WARNING: more than one match found  (" + dynlen(dps) + ") for DP request of type '" + type + "': " + dps + ". Returned the first one");
  }
  
  return dps[1];
}

/**
    @param type            what type of DPs are you interested in? e.g. HV_vMon, HV_Primary_vMon, HV_Primary_iMon...
    @param parameters      describes what DPs you are interested in e.g. for chamber you give side, station, ring and chamberNumber.
    @param localSystemOnly (optional, default=true) if this is true then the search is only performed on the local system, if false - on all csc systems.
    @return                an array of DP names that match the requested type and given parameters.
*/
dyn_string emu_getDpNames(string type, mapping parameters, dyn_string &exceptionInfo, bool localSystemOnly = true) {
  dyn_string ret;
  
  string pattern = emu_getMappingValue("dpNamePatterns", type, exceptionInfo);
  if (dynlen(exceptionInfo) > 0) {
    emu_errorHandled(exceptionInfo);  
    return ret;
  }
  
  string dp = emu_fillPattern(pattern, parameters);
  bool systemIncluded = false;
  if (!localSystemOnly) {
    string dpForSysSearch = dp;
    strreplace(dpForSysSearch, ":_", ""); // to eliminate possible colon used for configs
    systemIncluded = (strpos(dpForSysSearch, ":") >= 0); // check if system is included - have to do this nasty way because dpSubStr is too smart (if it finds the dp in the local system - gives back the local system name even if it's not included in the dp name..)
  }
  
  if (localSystemOnly || systemIncluded) { // if system name is included or the search is restricted to the local system only
    ret = dpNames(dp);
  } else {                    // if there's no sys name (most of the time this is the case) then search accross all connected systems
    ret = dpNames(emu_getCscSystemNamesPattern(exceptionInfo) + ":" + dp);
    if (emu_checkException(exceptionInfo)) { return makeDynString(); }
  }
  
  return ret;
}

/** Substitutes parameter values in the pattern. Parameters in patterns have dollar symbols on both sides of the parameter name.
  e.g. pattern "LowVoltage/CSC_ME_$side$$station$$ring$_C$chamberNumber$_LV".
  Note any remaining $ parameters which were not found in the parameters mapping are replaced by a wildcard - "*".
*/
string emu_fillPattern(string pattern, mapping parameters) {
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

/** Returns an array having the name provided (looks up in DPs of type EMU_array). */
dyn_string emu_getArray(string name, dyn_string &exceptionInfo) {
  //check the cache
  if (mappingHasKey(emu_g_arrayCache, name)) {
    return emu_g_arrayCache[name];
  }
  
  string dpName = "emu_array_" + name;
  if (!dpExists(dpName)) {
    emu_addError("Requested array \"" + name + "\" does not exist", exceptionInfo);
    return makeDynString();
  }

  dyn_string data;
  dpGet(dpName + ".array", data);
  
  emu_g_arrayCache[name] = data;
      
  return data;
}

string emu_getCscSystemNamesPattern(dyn_string &exceptionInfo) {
  if (strlen(emu_g_cscSystemNamesPattern) == 0) {
    mapping dcsProjects = emu_getMapping("dcsProjectSystemNames", exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return ""; }
    emu_g_cscSystemNamesPattern = "{";
    for (int i=1; i <= mappinglen(dcsProjects); i++) {
      string sysName = mappingGetValue(dcsProjects, i);
      if (i > 1) {
        emu_g_cscSystemNamesPattern += ",";
      }
      emu_g_cscSystemNamesPattern += sysName;
    }
    if (!mappingHasKey(dcsProjects, getSystemId())) {
      string mySysName = getSystemName();
      strreplace(mySysName, ":", "");
      emu_g_cscSystemNamesPattern += "," + mySysName;
    }
    emu_g_cscSystemNamesPattern += "}";
  }
  return emu_g_cscSystemNamesPattern;  
}

/** Parses the "parameters string" and returns a mapping.
  String syntax is: key1=value1;key2=value2;.... */
mapping emu_parseParameterString(string strParams, dyn_string &exceptionInfo) {
  dyn_string entries;
  mapping ret;
  
  entries = strsplit(strParams, ";");
  for (int i=1; i <= dynlen(entries); i++) {
    dyn_string entry = strsplit(entries[i], "=");
    if (dynlen(entry) != 2) {
      emu_addError("Could not parse the parameterString: " + strParams, exceptionInfo);
      return;
    }
    ret[entry[1]] = entry[2];
  }
  
  return ret;
}

/** Converts parameter mapping to string with syntax: key1=value1;key2=value2;.... */
string emu_mappingToParameterString(mapping params) {
  string ret;
  
  for (int i=1; i <= mappinglen(params); i++) {
    string key = mappingGetKey(params, i);
    string value = params[key];
    
    if (i > 1) {
      ret += ";";
    }
    ret += key + "=" + value;
  }
  
  return ret;
}
