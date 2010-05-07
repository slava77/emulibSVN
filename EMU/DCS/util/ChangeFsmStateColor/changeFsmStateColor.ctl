global const string state = "STANDBY";
global const string color = "FwStateOKNotPhysics"; //FwStateAttention1;

main() {
  dyn_string fsmTypeDps = dpNames("*:*", "_FwFsmObjectType");
  for (int typeIndex=1; typeIndex <= dynlen(fsmTypeDps); typeIndex++) {
    dyn_string states;
    dpGet(fsmTypeDps[typeIndex] + ".states", states);
    for (int i=1; i <= dynlen(states); i++) {
      if (states[i] == state) {
        DebugTN("Setting standby color to " + color + " for FSM type " + fsmTypeDps[typeIndex]);
        states[i+1] = color;
        //dpSetWait(fsmTypeDps[typeIndex] + ".states", states);
        
//         DebugTN(states);
//         DebugTN("");
//         DebugTN("");
      }
    }
  }
}
