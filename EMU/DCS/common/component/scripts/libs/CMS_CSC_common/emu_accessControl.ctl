#uses "fwAccessControl/cmsAccessControlSettings.ctc"

/** Returns true if currently logged in user has control privileges. */
bool emu_hasControl() {
  dyn_string ex;
  bool hasControl;
  fwAccessControl_isGranted("CMS_CSC:Control", hasControl, ex);
  if (emu_checkException(ex)) { return false; }

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

/**
  * Returns true if the current user can control the given fsm node, false if not.
  * @param fsmNode The CU/LU node name optionally followed by "::" + the name of the Object/Device. If this is an empty string, true is returned.
  */
bool emu_hasFsmControl(string fsmNode) {
  if (fsmNode == "") {
    return true;
  }
  return (fwCU_getOperationMode(fsmNode) == 1);
}

/** This function should be called from fwAccessControl callback (registered by fwAccessControl_setupPanel()).
    This function figures out if the user privileges and disables or enables control and expert control buttons.
    @param fsmNodeToLookAt - fsm node which should be checked for control ownership (if the current user cannot control this FSM node, the access will be denied to the buttons). Format: The CU/LU node name optionally followed by "::" + the name of the Object/Device.
    @param controlButtons - an array of control button names
    @param changeVisible - if false then controls are only disabled/enabled, if true then controls are made invisible/visible
    @param noControlInfoLabel - (optional) name of the shape to be displayed when the user has no control (for information)
    @param expertControlButtons - (optional) an array of controls which require expert privilege
    @param changeExpertButtonsVisible - (optional) if false then expert controls are only disabled/enabled, if true then they're made invisible/visible
    @param initialDisabling - (optional) if this is true then privileges are not checked and all controls are disabled
*/
void emu_enableDisableControl(string fsmNodeToLookAt, dyn_string controlButtons, bool changeVisible, string noControlInfoLabel = "", 
                              dyn_string expertControlButtons = makeDynString(), bool changeExpertButtonsVisible = true,
                              bool initialDisabling = false) {
  bool enableControl = false;
  bool enableExpertControl = false;  
  
  if (!initialDisabling) {
    enableControl = (emu_hasControl() && emu_hasFsmControl(fsmNodeToLookAt));
    enableExpertControl = (emu_hasExpertControl() && emu_hasFsmControl(fsmNodeToLookAt));
  }
  
  if (isFunctionDefined("getCurrentFsmOwners")) {
    string user;
    fwAccessControl_getUserName(user);
    dyn_string fsmOwners = getCurrentFsmOwners();
    emu_info("found fsm owners: " + fsmOwners);
    emu_info("currently logged in user: " + user);
    emu_info("can this user have control? " + dynContains(fsmOwners, user));
  } else {
    emu_info("could not determine FSM owners - ignoring");
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
