#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"

/**@file

HV Commands

@author Evaldas Juska (PH/UCM)
@date   March 2011
*/

/**
  * Initiates or closes a given alert.
  * @param args[1] - alert name (dp ExsysAlert_<alert_name> of type ExSys_alert must exist)
  * @param args[2] - alert state; false = off, true = on.
  */
public void exsysCmd_alert(dyn_string args) {
  if (dynlen(args) < 2) {
    return;
  }
  string dp = "ExsysAlert_" + args[1];
  bool isOn = args[2];
  
  if (!dpExists(dp)) {
    return;
  }
  
  dpSetWait(dp + ".value", isOn);
}
