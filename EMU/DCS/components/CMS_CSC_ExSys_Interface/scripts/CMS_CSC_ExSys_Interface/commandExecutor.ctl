#uses "aes.ctl"
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/commands/hvCommands.ctl"

private const string EXSYS_COMMAND_DP = "ExSys_command.value";
private const string EXSYS_COMMAND_FUNCTION_PREFIX = "exsysCmd_";

main()
{
  dyn_string ex;
  string commandDpSystem = "";
  if (getHostname() != "macfrank") {
    dyn_string sysNames;
    fwInstallation_getApplicationSystem("CMS_CSCfw_Supervisor", sysNames);
    if (dynlen(sysNames) > 0) {
      commandDpSystem = sysNames[1];
    }
  }
  
  if (!dpExists(commandDpSystem + EXSYS_COMMAND_DP)) {
    emu_errorSingle("ExSys commandExecutor: ExSys command DP doesn't exist: " + commandDpSystem + EXSYS_COMMAND_DP + ". exiting....");
    return;
  }
  dpConnect("exsys_executeCommandCB", false, commandDpSystem + EXSYS_COMMAND_DP);
  emu_info("ExSys commandExecutor started, listening for commands from the expert system");
}

/** 
  * Callback for EXSYS_COMMAND_DP changes - unpacks the command function name and arguments and executes the function.
  * All command functions are defined in CMS_CSC_ExSys_Interface/commands package.
  * All command functions must accept a dyn_string as their only parameter.
  */
void exsys_executeCommandCB(string dp, string command) {
  dyn_string commandSplit = strsplit(command, "|");
  if (dynlen(commandSplit) < 1) {
    return;
  }
  string functionName = EXSYS_COMMAND_FUNCTION_PREFIX + commandSplit[1];
  dynRemove(commandSplit, 1);
  dyn_string args = commandSplit;
  
  if (!isFunctionDefined(functionName)) {
    emu_errorSingle("ExSys commandExecutor: command function '" + functionName + "' doesn't exist!");
    return;
  }
  
  emu_info("ExSys commandExecutor: executing command function '" + functionName + "' with these arguments: " + args);
  startThread(functionName, args);
}

public void exsysCmd_test(dyn_string args) {
  emu_info("This is a test command from Expert System. Got these arguments: " + args);
}
