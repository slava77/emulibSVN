#uses "fwAccessControl/cmsAccessControlSettings.ctc"

/** Returns true if currently logged in user has control privileges. */
bool emu_hasControl() {
  dyn_string ex;
  bool hasControl;
  fwAccessControl_isGranted("CMS_CSC:Control", hasControl, ex);
  if (emu_checkException(ex)) { return false; }

  //TODO check if this user has FSM control and is the owner of it.  
  
  return hasControl;
}

/** Returns true if currently logged in user has expert control privileges. */
bool emu_hasExpertControl() {
  dyn_string ex;
  bool isExpert;
  fwAccessControl_isGranted("CMS_CSC:Debug", isExpert, ex);
  if (emu_checkException(ex)) { return false; }
  return isExpert;
}

/** This function should be called from fwAccessControl callback (registered by fwAccessControl_setupPanel()).
    This function figures out if the user privileges and disables or enables control and expert control buttons.
    @param controlButtons - an array of control button names
    @param changeVisible - if false then controls are only disabled/enabled, if true then controls are made invisible/visible
    @param noControlInfoLabel - (optional) name of the shape to be displayed when the user has no control (for information)
    @param expertControlButtons - (optional) an array of controls which require expert privilege
    @param changeExpertButtonsVisible - (optional) if false then expert controls are only disabled/enabled, if true then they're made invisible/visible
    @param initialDisabling - (optional) if this is true then privileges are not checked and all controls are disabled
*/
void emu_enableDisableControl(dyn_string controlButtons, bool changeVisible, string noControlInfoLabel = "", 
                              dyn_string expertControlButtons = makeDynString(), bool changeExpertButtonsVisible = true,
                              bool initialDisabling = false) {
  bool enableControl = false;
  bool enableExpertControl = false;
  
  if (!initialDisabling) {
    enableControl = emu_hasControl();
    enableExpertControl = emu_hasExpertControl();
  }
  
  string controlProperty = "enabled";
  if (changeVisible) {
    controlProperty = "visible";
  }
  
  for (int i=1; i <= dynlen(controlButtons); i++) {
    setValue(controlButtons[i], controlProperty, enableControl);
  }
  
  string expertControlProperty = "enabled";
  if (changeExpertButtonsVisible) {
    expertControlProperty = "visible";
  }

  for (int i=1; i <= dynlen(expertControlButtons); i++) {
    setValue(expertControlButtons[i], expertControlProperty, enableExpertControl);
  }
  
  if (noControlInfoLabel != "") {
    setValue(noControlInfoLabel, "visible", !enableControl);
  }
}
