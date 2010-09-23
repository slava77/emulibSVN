#uses "CMS_CSC_ExSys_Interface/httpHandling.ctl"
#uses "CMS_CSC_ExSys_Interface/msgHandling.ctl"

bool exsys_sendFactCollection(int factCollectionDocId, dyn_string &exceptionInfo) {
  string factCollection = exsys_getFactCollection(factCollectionDocId, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  exsys_httpPost(factCollection, exceptionInfo);
  emu_debug("Exsys: Fact sent successfully");
}
