// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_main.ctl"
#uses "CMS_CSC_UI/emuui_configuration.ctl"
#uses "CMS_CSC_UI/emuui_sessionManager.ctl"
#uses "CMS_CSC_UI/emuui_utils.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"

/**@file

This package contains functions related graphics.
@author Evaldas Juska (PH/UCM)
@date   July 2009
*/

const int EMUUI_SMALL_FSM_BUTTON_WIDTH = 60;
const int EMUUI_SMALL_FSM_BUTTON_HEIGHT = 20;

const int EMUUI_SMALL_FSM_POSITION_FIX_OFFSET_X = -5;
const int EMUUI_SMALL_FSM_POSITION_FIX_OFFSET_Y = -6;

const int EMUUI_BIG_FSM_POSITION_FIX_OFFSET_X = 18;
const int EMUUI_BIG_FSM_POSITION_FIX_OFFSET_Y = 82;

/** Adds FSM node at the specified location and adds the reference names of the included objects to the "references" array. */
void emuui_addFsmNode(string node, int x, int y, dyn_string &references, bool smallVersion = false, string parent = "") {
  string object;
  if ((parent == "") || (parent == node)) {
    object = _fwCU_getNodeObj(node);
  } else {
    object = node + "::" + node;
    node = parent;
  }

  if (!smallVersion) { // full button
    y += EMUUI_BIG_FSM_POSITION_FIX_OFFSET_Y;  // hmm positioning is screwed up somewhere
    x += EMUUI_BIG_FSM_POSITION_FIX_OFFSET_X;
    
    string refName = "ref_FSMobj_" + node;
    while (shapeExists(refName)) { refName += "_duplicate_1"; }
    dynAppend(references, refName);

    addSymbol(myModuleName(), myPanelName(), EMUUI_OBJECTS_DIR + "/refFSMobj.pnl", 
              refName, makeDynString("$node:" + node, "$obj:" + object), x, y, 0, 1, 1);
  } else {
    y += EMUUI_SMALL_FSM_POSITION_FIX_OFFSET_Y;  // hmm positioning is screwed up somewhere
    x += EMUUI_SMALL_FSM_POSITION_FIX_OFFSET_X;

    _emuui_addFsmObjButton(node, object, x, y, references);
    if (fwFsm_isCU(node, object)) {
      _emuui_addFsmLockButton(node, object, x + EMUUI_SMALL_FSM_BUTTON_WIDTH - 2, y, references);
    } else {
      _emuui_addFsmDevLockButton(node, object, x + EMUUI_SMALL_FSM_BUTTON_WIDTH - 2, y, references);
    }
  }
}

/** Adds FSM obj button and returns refName. */
string _emuui_addFsmObjButton(string node, string obj, int x, int y, dyn_string &references) {
  dyn_string params;
  string refName = obj + "_button";
  while (shapeExists(refName)) { refName += "_duplicate_1"; }
  dynAppend(references, refName);
  
  params = makeDynString("$domain:" + node, "$obj:" + obj, "$part:");
  addSymbol(myModuleName(), myPanelName(), "fwFSM/ui/fwSmiObj.pnl", refName, params, x, y, 0, 1, 1);
        
  // set the size
  setValue(refName + ".CascadeButton1", "size", EMUUI_SMALL_FSM_BUTTON_WIDTH, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
        
  return refName;
}

/** Adds FSM device lock button and returns refName. */
string _emuui_addFsmDevLockButton(string node, string obj, int x, int y, dyn_string &references) {
  dyn_string params;
  string refName = obj + "_devLockButton";
  while (shapeExists(refName)) { refName += "_duplicate_1"; }
  dynAppend(references, refName);
  
  params = makeDynString("$domain:" + node, "$obj:" + obj, "$part:");
  addSymbol(myModuleName(), myPanelName(), "fwFSM/ui/fwFsmDevLock.pnl", refName, params, x, y, 0, 1, 1);

  // set the size
  setValue(refName + ".disabled_grey", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".enabled_grey", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".disabled", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".enabled", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
        
  return refName;
}

/** Adds FSM device lock button and returns refName. */
string _emuui_addFsmLockButton(string node, string obj, int x, int y, dyn_string &references) {
  dyn_string params;
  string refName = obj + "_lockButton";
  while (shapeExists(refName)) { refName += "_duplicate_1"; }
  dynAppend(references, refName);
  
  params = makeDynString("$domain:" + node, "$obj:" + obj);
  addSymbol(myModuleName(), myPanelName(), "fwFSM/ui/fwFsmLock.pnl", refName, params, x, y, 0, 1, 1);
        
  // set the size
  setValue(refName + ".lock1", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock2", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock3", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock4", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock5", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock6", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock7", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock8", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
  setValue(refName + ".lock9", "size", EMUUI_SMALL_FSM_BUTTON_HEIGHT, EMUUI_SMALL_FSM_BUTTON_HEIGHT);
        
  return refName;
}

void emuui_alignSmallFsmNodes(dyn_string fsmReferences, dyn_string fsmRefPointShapes) {
  int nodeCount = dynlen(fsmRefPointShapes);
  for (int i=1; i <= nodeCount; i++) {
    if (dynlen(fsmReferences) < i * 2 - 1) {
      emu_debug("Some FSM nodes missing in emuui_alignSmallFsmNodes");
      return;
    }
    int refPosX, refPosY;
    getValue(fsmRefPointShapes[i], "position", refPosX, refPosY);
    refPosX += EMUUI_SMALL_FSM_POSITION_FIX_OFFSET_X - 1;
    refPosY += EMUUI_SMALL_FSM_POSITION_FIX_OFFSET_Y - 1;
    setValue(fsmReferences[i * 2 - 1], "position", refPosX, refPosY);
    refPosX += EMUUI_SMALL_FSM_BUTTON_WIDTH - 2;
    setValue(fsmReferences[i * 2], "position", refPosX, refPosY);
  }
}

/** Positions and shows a warning object within a tooltip and resizes the tooltip background. */
void emuui_showTooltipWarning(shape refPointShp, shape warnShp, shape backgroundShp) {
  // if it's already visible - do nothing
  if (warnShp.visible) {
    return;
  }
  
  int posX, posY, sizeX, sizeY;
  getValue(refPointShp, "position", posX, posY);
  getValue(backgroundShp, "size", sizeX, sizeY);  
  
  //extend the background to cover the warning
  int warnSizeX, warnSizeY;
  getValue(warnShp, "size", warnSizeX, warnSizeY);
  setValue(backgroundShp, "size", sizeX, sizeY + warnSizeY + 3);
  
  //position the warning under where background ends
  int warnPosX, warnPosY;
  getValue(warnShp, "position", warnPosX, warnPosY);
  int warnPosY = posY + sizeY;
  setValue(warnShp, "position", warnPosX, warnPosY);
  
  warnShp.visible = true;
  
  dpSetWait(emuui_getSessionDp("tooltipSizeUpdated"), 1);
}

/** Hides a warning object within a tooltip and resizes the tooltip background. */
void emuui_hideTooltipWarning(shape warnShp, shape backgroundShp, dyn_string otherWarningShapes = makeDynString()) {
  // if it's already invisible - do nothing
  if (!warnShp.visible) {
    return;
  }

  int sizeX, sizeY;
  getValue(backgroundShp, "size", sizeX, sizeY);  
  
  // shrink the background size
  int warnSizeX, warnSizeY;
  getValue(warnShp, "size", warnSizeX, warnSizeY);
  setValue(backgroundShp, "size", sizeX, sizeY - warnSizeY - 3);
  
  int warnPosX, warnPosY;
  if (dynlen(otherWarningShapes) > 0) {
    getValue(warnShp, "position", warnPosX, warnPosY);
  }

  warnShp.visible = false;
  
  for (int i=1; i <= dynlen(otherWarningShapes); i++) {
    if (otherWarningShapes[i] == warnShp) {
      continue;
    }
    int objPosX, objPosY;
    getValue(otherWarningShapes[i], "position", objPosX, objPosY);
    if (objPosY > warnPosY) {
      setValue(otherWarningShapes[i], "position", objPosX, objPosY - warnSizeY - 3);
    }
  }
  
  dpSetWait(emuui_getSessionDp("tooltipSizeUpdated"), 1);
}
/** move objects provided in the shapes array relatively to a reference object movement.
  @param objects        Graphics objects to be moved.
  @param newRefObjX     X coordinate of the reference object after movement.
  @param newRefObjY     Y coordinate of the reference object after movement.
  @param oldRefObjX     X coordinate of the reference object before movement.
  @param oldRefObjY     Y coordinate of the reference object before movement.
*/
void emuui_repositionObjectRelatively(dyn_string objects, int oldRefObjX, int oldRefObjY, int newRefObjX, int newRefObjY) {
  int deltaX = newRefObjX - oldRefObjX,
      deltaY = newRefObjY - oldRefObjY;
  
  int objPosX, objPosY;
  for (int i=1; i <= dynlen(objects); i++) {
    getValue(objects[i], "position", objPosX, objPosY);
    setValue(objects[i], "position", objPosX + deltaX, objPosY + deltaY);
  }
}

/** Corrects a given alarm color e.g. if the color is empty string - it's changed to OK color. */
string emuui_correctAlarmColor(string color, string noAlarmColor = "FwStateOKPhysics") {
  if (strlen(color) == 0) {
    return "FwStateOKPhysics";
  }
  if (color == "FwAlarmErrorAck") {
    return "FwAlarmFatalAck";
  }
  
  return color;
}

/** Fills the given table with "???" and optionally makes the background grey (if greyOut is true).
    This is used to indicate that there's no communication. */
void emuui_questionOutTable(string tableName, bool greyOut = true, 
                            dyn_int excludeColumns = makeDynInt(),
                            dyn_int excludeRows = makeDynInt()) {
  emuui_fillTable(tableName, "???", "FwEquipmentDisabled", greyOut, excludeColumns, excludeRows);
}

/** Fills the given table with provided text and optionally changes the background to a given color (if changeColor is true). 
    Columns provided in excludeColumns array and rows provided in excludeRows array are not touched */
void emuui_fillTable(string tableName, string text, string color = "white", bool changeColor = true, 
                            dyn_int excludeColumns = makeDynInt(),
                            dyn_int excludeRows = makeDynInt()) {
  int rowCount;
  int columnCount;
  shape table = getShape(tableName);
  getValue(table, "lineCount", rowCount);
  getValue(table, "columnCount", columnCount);
  for (int row=0; row < rowCount; row++) {
    if (dynContains(excludeRows, row)) { continue; }
    for (int col=0; col < columnCount; col++) {
      if (dynContains(excludeColumns, col)) { continue; }
      string columnName;
      getValue(table, "columnToName", col, columnName);
      setValue(table, "cellValueRC", row, columnName, text);
      if (changeColor) {
        setValue(table, "cellBackColRC", row, columnName, color);
      }
    }
  }
}

/** Changes background color of a given row in a table (it's crazy that there's no such function in PVSS, isn't it). */
void emuui_tableRowBackColor(string tableName, int rowNum, string color) {
  int columnCount;
  shape table = getShape(tableName);
  getValue(table, "columnCount", columnCount);
  for (int col=0; col < columnCount; col++) {
    string columnName;
    getValue(table, "columnToName", col, columnName);
    setValue(table, "cellBackColRC", rowNum, columnName, color);
  }
}
