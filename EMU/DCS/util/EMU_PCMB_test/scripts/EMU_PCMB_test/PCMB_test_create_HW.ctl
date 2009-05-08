main() {
  dyn_string ex;
  string mainBus = "LVCB_2";
  
  //create CANbus
  dyn_mixed canBusData;
  canBusData[3] = mainBus;
  canBusData[4] = "";
  emucdb_createElmbCANbus(canBusData, ex);
  if (emu_checkException(ex)) { return; }
  
  //create ELMBs
  dyn_mixed elmbNodeData;
  elmbNodeData[2] = mainBus;
  elmbNodeData[4] = "";
  for (int i=1; i < 64; i++) {
    elmbNodeData[3] = "ELMB_" + emu_decToHex(i, 1);
    emucdb_createElmbNode(elmbNodeData, ex);
    if (emu_checkException(ex)) { return; }
    
    //create AI
    dyn_mixed aiData;
    aiData[2] = "ELMB" + fwDevice_HIERARCHY_SEPARATOR +
                elmbNodeData[2] + fwDevice_HIERARCHY_SEPARATOR + elmbNodeData[3] + 
                fwDevice_HIERARCHY_SEPARATOR + "AI";
    aiData[4] = "";
    for (int j=0; j <= 21; j++) {
      aiData[3] = "ai_" + j;
      emucdb_createElmbAi(aiData, ex);
      if (emu_checkException(ex)) { return; }
    }
    for (int j=32; j <= 35; j++) {
      aiData[3] = "ai_" + j;
      emucdb_createElmbAi(aiData, ex);
      if (emu_checkException(ex)) { return; }
    }
    for (int j=48; j <= 57; j++) {
      aiData[3] = "ai_" + j;
      emucdb_createElmbAi(aiData, ex);
      if (emu_checkException(ex)) { return; }
    }
    
    //create DO
    dyn_mixed doData;
    doData[2] = aiData[2];
    doData[4] = doData[4];
    for (int j=0; j < 8; j++) {
      doData[3] = "do_A_" + j;
      emucdb_createElmbDo(doData, ex);
      if (emu_checkException(ex)) { return; }
      doData[3] = "do_C_" + j;
      emucdb_createElmbDo(doData, ex);
      if (emu_checkException(ex)) { return; }
    }
  }
}
