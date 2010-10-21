// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_main.ctl"
#uses "CMS_CSC_UI/emuui_configuration.ctl"
#uses "CMS_CSC_UI/emuui_sessionManager.ctl"
#uses "CMS_CSC_UI/emuui_utils.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"

// treeCache lib (needed for _treeCache_dpGetAll(...) in emu_minMax(...)
#uses "majority_treeCache/treeCache.ctl"

/**@file

This package contains utility functions for UI components.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

const int EMUUI_TABLE_BORDER_WIDTH = 4;
const int EMUUI_TABLE_HEADER_HEIGHT = 20; 
const int EMUUI_TABLE_BORDER_HEIGHT = 4;

const string EMUUI_HISTOGRAM_PLOT_DP = "histogramPlot";

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

/** returns stime of a given datapoint (as type time). */
string emuui_getLastUpdateTimeAsTime(string dp) {
  time stime;
  string dpSTime = dpSubStr(dp, DPSUB_SYS_DP_EL) + ":_online.._stime";
  if (!dpExists(dpSTime)) {
    return "";
  }
  dpGet(dpSTime, stime);

  return stime;
}

/** returns stime of a given datapoint in string. So this is the only place if you need to change the date format everywhere. */
string emuui_getLastUpdateTime(string dp) {
  time stime = emuui_getLastUpdateTimeAsTime(dp);
  string timeStr = emuui_formatTime(stime);
  return (string) timeStr;
}

/* Format time as a string. */
string emuui_formatTime(time t) {
  return formatTime("%Y.%m.%d %H:%M", t);
}

/** returns dp which has been updated most recently in the dp List. */
string emuui_getDpWithLastUpdateTime(dyn_string dpList) {
  int retIndex = 1;
  time lastUpdate;
  for (int i=1; i <= dynlen(dpList); i++) {
    time stime;
    string dpSTime = dpSubStr(dpList[i], DPSUB_SYS_DP_EL) + ":_online.._stime";
    dpGet(dpSTime, stime);
    if ((i == 1) || (stime > lastUpdate)) {
      lastUpdate = stime;
      retIndex = i;
    }
  }

  return dpList[retIndex];
}

/** returns stime of a given datapoint in string. So this is the only place if you need to change the date format everywhere. */
string emuui_getLastUpdateTimeOfDpList(dyn_string dpList) {
  time lastUpdate;
  for (int i=1; i <= dynlen(dpList); i++) {
    time stime;
    string dpSTime = dpSubStr(dpList[i], DPSUB_SYS_DP_EL) + ":_online.._stime";
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

void emuui_showInfoMessage(string message, string title = "Info") {
  ChildPanelOnCentralModal(EMUUI_OBJECTS_DIR + "/refMessage.pnl", 
                           title, 
                           makeDynString("$message:" + message));
}

void emuui_openBrowser(string url) {
  if (_WIN32) {
    system("start firefox.exe " + url + "\"");
  }else if (_UNIX) {
    system("firefox " + url);
  } else {
    emu_error("Unknown operating system - cannot start firefox");
  }
}

/**
  * Creates and shows a histogram for the given data (divides the histogram into <numBuckets> buckets)
  * @param dataDpes dpes to be plotted
  * @param numBuckets number of buckets to divide the data into
  * @param dataTitle title of the data that you're ploting (this will name the the plot to "<dataTitle> Plot" and set the legend to "<dataTitle>")
  */
void emuui_showHistogram(dyn_string dataDpes, int numBuckets, string dataTitle) {
  if (emuui_getSessionId() == "") {
    emu_errorSingle("CSC UI session is not initialized. Histograms are not supported outside CSC UI. Please run this panel from within CSC UI. Terminating.");
    return;
  }
  string histogramDp = emuui_getSessionDp("histogram");
  dyn_int histData;
  float rangeMin, rangeMax;
  emuui_getHistogramData(dataDpes, numBuckets, histData, rangeMin, rangeMax);
  dpSetWait(histogramDp + ".histogramData", histData, 
            histogramDp + ".rangeMin", rangeMin, 
            histogramDp + ".rangeMax", rangeMax);
  dyn_float xRangeMin, xRangeMax;
  if (!dpExists(EMUUI_HISTOGRAM_PLOT_DP)) {
    emu_errorSingle("Histogram plot DP (name = '" + EMUUI_HISTOGRAM_PLOT_DP + "', type='FwTrendingPlot') was not found, terminating.");
    return;
  }
  
  dpGet(EMUUI_HISTOGRAM_PLOT_DP + ".xRangeMin", xRangeMin, 
        EMUUI_HISTOGRAM_PLOT_DP + ".xRangeMax", xRangeMax);
  xRangeMin[1] = rangeMin;
  xRangeMax[1] = rangeMax;
  dpSetWait(EMUUI_HISTOGRAM_PLOT_DP + ".xRangeMin", xRangeMin, 
            EMUUI_HISTOGRAM_PLOT_DP + ".xRangeMax", xRangeMax);
  ChildPanelOnCentral("fwTrending/fwTrendingPlot.pnl", dataTitle + " Plot", 
                      makeDynString("$PlotName:" + EMUUI_HISTOGRAM_PLOT_DP,
                                    "$templateParameters:histDataDpe=" + dpSubStr(histogramDp, DPSUB_DP_EL) + ".histogramData,dataTitle=" + dataTitle + ","));
}

/**
  * Computes data for a histogram. Takes values from the given DPEs, finds min and max of the range and divides that range to numIntervals intervals 
  * and fills in the histData with number of DPEs that fall into each interval.
  * @param dataDpes data DPEs - all must be of numeric type
  * @param numBuckets number of intervals (buckets) to divide the total range of data into
  * @param histData number of DPEs that fall into each interval
  * @param rangeMin start of the histogram data range
  * @param rangeMax end of the histogram data range
  */
void emuui_getHistogramData(dyn_string dataDpes, int numBuckets, dyn_int &histData, float &rangeMin, float &rangeMax) {
  dyn_float data;
  if (isFunctionDefined("_treeCache_dpGetAll")) {
    _treeCache_dpGetAll(dataDpes, data); // allows mass dpGet from different systems
  } else {
    dpGet(dataDpes, data); // if the treeCache library is not available, try doing dpGet()
  }
  emu_minMax(data, rangeMin, rangeMax);
  rangeMin -= (rangeMax - rangeMin) * 0.1; // expand the range by 20%
  rangeMax += (rangeMax - rangeMin) * 0.1; // expand the range by 20%
  
  // initialize histogram data
  histData;
  for (int i=1; i <= numBuckets; i++) {
    histData[i] = 0;
  }
  
  float bucketSize = (rangeMax - rangeMin) / (float) numBuckets;
  // here we go
  for (int i=1; i <= dynlen(data); i++) {
    // get the bucket index (0-based) and convert to the stupid PVSS 1-based array index
    int bucketIdx = floor((data[i] - rangeMin) / bucketSize) + 1;
    histData[bucketIdx]++;
  }
}
