#uses "CMS_CSC_ExSys_Interface/httpHandling.ctl"
#uses "CMS_CSC_ExSys_Interface/msgHandling.ctl"

/** Time window (in milliseconds) during which facts to be sent are packaged into one fact collection. */
private const int EXSYS_AUTO_PACKAGING_WINDOW = 2000;

/** XML document ID of the fact collection that is currently open for auto packaging. */
private global int exsys_autoPackagingCollectionId = -1;
private global int exsys_autoPackagingTimeoutThread = -1;

/** Sends out the given fact collection. Internal use one - users should use exsys_sendFact(...) instead. */
private void exsys_sendFactCollection(int factCollectionDocId, dyn_string &exceptionInfo) {
  string factCollection = exsys_getFactCollection(factCollectionDocId, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { 
    emu_addError("Failed to send out this fact collection: " + factCollection);
    return;
  }
  exsys_httpPost(factCollection, exceptionInfo);
  emu_debug("Exsys: Fact sent successfully");
}

/**
  * Creates a fact and puts it into the provided fact collection.
  * @param type type of the fact (or fact name) e.g. dcsFsmStateFact or dcsAlertFact, etc... (please look up valid fact names in the expert system)
  * @param timestamp timestamp for the fact
  * @param component component which this fact is providing info about e.g. ME+1/1/1, ME+1/1/1/DMB, etc.. (look up valid component names in the expert system component ontology)
  * @param severity severity level of the fact - please use the EXSYS_FACT_SEVERITY_* constants e.g. EXSYS_FACT_SEVERITY_INFO, EXSYS_FACT_SEVERITY_ERROR, etc..
  * @param description description of the fact
  * @param factParamNames names of the parameters of the fact i.e. elements that are specific to this fact type (look up specific fact definition in the expert system)
  * @param factParamValues values of the parameters supplied in the factParamNames parameter
  * @param autoPackaging (optional, default = true) if this is false, then the fact is packaged alone in a new fact collection and sent out immediately. If this is true - fact is autopackaged, which means that it could be packaged into a fact collection together with other facts in EXSYS_AUTO_PACKAGING_WINDOW ms time window.
  */
public void exsys_sendFact(string type, time timestamp, string component, string severity, string description, 
                     dyn_string factParamNames, dyn_anytype factParamValues, dyn_string &exceptionInfo, bool autoPackaging = true) {
  // no auto packaging - send the fact out immediately
  if (!autoPackaging) {
    int factCollectionId = exsys_createFactCollection(exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    exsys_createFact(factCollectionId, type, timestamp, component, severity, description, factParamNames, factParamValues, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    exsys_sendFactCollection(factCollectionId, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  } else { // AUTO PACKAGING
    synchronized (exsys_autoPackagingCollectionId) {
      if (exsys_autoPackagingCollectionId == -1) { // no open collection exists - create a new one
        exsys_autoPackagingCollectionId = exsys_createFactCollection(exceptionInfo);
        if (emu_checkException(exceptionInfo)) { return; }
        exsys_autoPackagingTimeoutThread = startThread("startAutoPackagingTimeoutThread");
      }
      // check the size of it - PVSS is not able to send out more than some 48000 symbols (lets say our safe limit for adding one more fact will be 40000 symbols)
      // if it's already more than 40000 symbols, let's ship it away immediately and open a new fact collection for auto packaging.
      if (exsys_getFactCollectionLength(exsys_autoPackagingCollectionId) > 40000) {
        stopThread(exsys_autoPackagingTimeoutThread);
        int fcId = exsys_autoPackagingCollectionId;
        exsys_autoPackagingCollectionId = -1;
        exsys_sendFactCollection(fcId, exceptionInfo);
        if (emu_checkException(exceptionInfo)) { return; }
        exsys_autoPackagingCollectionId = exsys_createFactCollection(exceptionInfo);
        if (emu_checkException(exceptionInfo)) { return; }
        exsys_autoPackagingTimeoutThread = startThread("startAutoPackagingTimeoutThread");
      }
      exsys_createFact(exsys_autoPackagingCollectionId, type, timestamp, component, severity, description, factParamNames, factParamValues, exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
    }
  }
}

/**
  * Thread that waits for EXSYS_AUTO_PACKAGING_WINDOW ms and then sends out the open auto-packaging fact collection.
  * Sending is synchronized on exsys_autoPackagingCollectionId (same as autopackaging handling block in exsys_sendFact(...)).
  */
private void startAutoPackagingTimeoutThread() {
  delay(0, EXSYS_AUTO_PACKAGING_WINDOW);
  synchronized(exsys_autoPackagingCollectionId) {
    if (exsys_autoPackagingCollectionId == -1) {
      return;
    }      
    int fcId = exsys_autoPackagingCollectionId;
    exsys_autoPackagingCollectionId = -1;
    dyn_string exceptionInfo;
    exsys_sendFactCollection(fcId, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
}
