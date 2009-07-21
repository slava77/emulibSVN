/**@file

This package contains utility functions for UI components.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

const int EMUUI_TABLE_BORDER_WIDTH = 4;
const int EMUUI_TABLE_HEADER_HEIGHT = 20; 
const int EMUUI_TABLE_BORDER_HEIGHT = 4;

/** Sets the table width to fit the columns and adjusts height for numOfRows given (if -1 then height is unchanged). */
void emuui_tableCompact(string tableShapeName, int numOfRows = -1) {
  shape table = getShape(tableShapeName);
  int columnCount = table.columnCount();
  int newTableWidth = 0, newTableHeight;
  for (int i=0; i < columnCount; i++) {
    newTableWidth += table.columnWidth(i);
  }
  newTableWidth += EMUUI_TABLE_BORDER_WIDTH;
  if (table.rowHeaderVisibility()) { // if row header is visible - add the row header width
    newTableWidth += table.rowHeaderWidth();
  }
  
  if (numOfRows != -1) { // compute new height
    newTableHeight = numOfRows * table.rowHeight() + EMUUI_TABLE_BORDER_HEIGHT;
    if (table.columnHeaderVisibility()) { // if header is visible - add EMUUI_TABLE_HEADER_HEIGHT
      newTableHeight += EMUUI_TABLE_HEADER_HEIGHT;
    }
  } else { // don't change the height
    int actualWidth, actualHeight;
    getValue(table, "size", actualWidth, actualHeight);
    newTableHeight = actualHeight;
  }
  
  table.size(newTableWidth, newTableHeight);
}

/** returns stime of a given datapoint in string. So this is the only place if you need to change the date format everywhere. */
string emuui_getLastUpdateTime(string dp) {
  time stime;
  string dpSTime = dpSubStr(dp, DPSUB_SYS_DP_EL_CONF) + ".._stime";
  if (!dpExists(dpSTime)) {
    return "";
  }
  dpGet(dpSTime, stime);

  string timeStr = formatTime("%Y.%m.%d %H:%M", stime);

  return (string) timeStr;
}

/** returns stime of a given datapoint in string. So this is the only place if you need to change the date format everywhere. */
string emuui_getLastUpdateTimeOfDpList(dyn_string dpList) {
  time lastUpdate;
  for (int i=1; i <= dynlen(dpList); i++) {
    time stime;
    string dpSTime = dpSubStr(dpList[i], DPSUB_SYS_DP_EL_CONF) + ".._stime";
    dpGet(dpSTime, stime);
    if ((i == 1) || (stime > lastUpdate)) {
      lastUpdate = stime;
    }
  }

  string timeStr = formatTime("%Y.%m.%d %H:%M", lastUpdate);

  return (string) timeStr;
}

/** Destroy reference panels given in the array. */
void emuui_destroyReferences(dyn_string references) {
  for (int i=1; i <= dynlen(references); i++) {
    removeSymbol(myModuleName(), myPanelName(), references[i]);
  }
}

/** Returns system name of the given DP without the colon at the end. */
string emuui_getSystemName(string dp) {
  string sysName = dpSubStr(dp, DPSUB_SYS);
  strreplace(sysName, ":", "");
  return sysName;
}

/** Makes first letters of all the words in the string capital. */
string emuui_capitalizeFirstLetters(string str) {
  dyn_string split = strsplit(str, " ");
  for (int i=1; i <= dynlen(split); i++) {
    string firstLetter = strtoupper((string) split[i][0]);
    split[i] = firstLetter + substr(split[i], 1);
  }
  string ret;
  for (int i=1; i <= dynlen(split); i++) {
    if (i > 1) { ret += " "; }
    ret += split[i];
  }
  return ret;
}
