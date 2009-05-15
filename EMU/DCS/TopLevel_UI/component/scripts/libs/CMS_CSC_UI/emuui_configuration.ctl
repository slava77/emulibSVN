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
    return;
  }

  dyn_string data;
  dpGet(dpName + ".map", data);
  
  mapping ret;
  dyn_string tmpSplit;
  for (int i=1; i <= dynlen(data); i++) {
    tmpSplit = strsplit(data[i], ";");
    if (dynlen(tmpSplit) != 2) {
      emu_addError("Lines #" + i + " in mapping \"" + name + version + "\" is corrupted", exceptionInfo);
      return;
    }
    if (mappingHasKey(ret, tmpSplit[1])) {
      emu_addError("Key \"" + tmpSplit[1] + "\" in mapping \"" + name + version + "\" is defined multiple times", exceptionInfo);
      return;
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
//     emu_addError("Key \"" + key + "\" does not exist in mapping \"" + name + version + "\"", exceptionInfo);
    return "";
  }
  return map[key];
}

void emuui_clearMappingCache() {
  mappingClear(emuui_g_mappingCache);
}

/** 
    @param parameters    describes what FSM node you are interested to e.g. for chamber you give side, station, ring and chamberNumber.
    @return an FSM node of a requested type and matching the parameters given.
*/
string emuui_getFsmNode(string type, mapping parameters, dyn_string &exceptionInfo) {
  string pattern = emuui_getMappingValue("fsmNodePatterns", type, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  for(int i=1; i <= mappinglen(parameters); i++) {
    string key = mappingGetKey(parameters, i);
    string value = parameters[key];
    
    strreplace(pattern, "$" + key + "$", value);
  }
  
  return pattern;
}
