#uses "CtrlXml"
#uses "CMS_CSC_common/emu_common.ctl"

private const string EXSYS_NAMESPACE = "http://www.cern.ch/cms/csc/dw/model";
private const string EXSYS_NAMESPACE_SHORT = "csc";
private const string EXSYS_DCS_SOURCE = "DCSFactService";

public const string EXSYS_FACT_SEVERITY_DEBUG = "DEBUG";
public const string EXSYS_FACT_SEVERITY_INFO = "INFO";
public const string EXSYS_FACT_SEVERITY_MINOR = "MINOR";
public const string EXSYS_FACT_SEVERITY_TOLERABLE = "TOLERABLE";
public const string EXSYS_FACT_SEVERITY_WARN = "WARN";
public const string EXSYS_FACT_SEVERITY_ERROR = "ERROR";
public const string EXSYS_FACT_SEVERITY_SEVERE = "SEVERE";
public const string EXSYS_FACT_SEVERITY_CRITICAL = "CRITICAL";
public const string EXSYS_FACT_SEVERITY_FATAL = "FATAL";

/**
  * Creates a new fact collection and returns the XML document ID.
  * @param requestId (optional) if provided, that is the fact request ID.
  * @return XML document ID of the fact collection.
  */
public int exsys_createFactCollection(dyn_string &exceptionInfo, long requestId = -1) {
  int docId = xmlNewDocument();
  
    int nodeId = xmlAppendChild(docId, -1, XML_ELEMENT_NODE, "input");
    xmlSetElementAttribute(docId, nodeId, "xmlns", "http://www.cern.ch/cms/csc/dw/ws/factCollectionInput");
    int parentNode = xmlAppendChild(docId, nodeId, XML_ELEMENT_NODE, "factCollection");
    xmlSetElementAttribute(docId, parentNode, "xmlns:" + EXSYS_NAMESPACE_SHORT, EXSYS_NAMESPACE);
    xmlSetElementAttribute(docId, parentNode, "xmlns", "");
  
//  int parentNode = _exsys_addXmlNode(docId, -1, "factCollection", "", exceptionInfo);
//  if (emu_checkException(exceptionInfo)) { return; }
  _exsys_addXmlNode(docId, parentNode, "source", EXSYS_DCS_SOURCE, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  if (requestId > -1) {
    _exsys_addXmlNode(docId, parentNode, "requestId", requestId, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  return docId;
}

/**
  * Renders the fact collection document as string and destroys the DOM tree - the given document ID is then no longer valid.
  * @param factCollectionDocId XML document ID of the fact collection that you want to obtain.
  * @return fact collection XML as string.
  */
public string exsys_getFactCollection(int factCollectionDocId, dyn_string &exceptionInfo) {
  string ret = xmlDocumentToString(factCollectionDocId);
  int retCode = xmlCloseDocument(factCollectionDocId);
  if (retCode == -1) {
    emu_addError("Exsys: Error while closing a fact collection XML document. DocID = " + factCollectionDocId, exceptionInfo);
    return ret;
  }
  return ret;
}

/**
  * Creates a fact and puts it into the provided fact collection.
  * @param factCollectionDocId XML document ID of the fact collection that this fact should be put into.
  * @param type type of the fact (or fact name) e.g. dcsFsmStateFact or dcsAlertFact, etc... (please look up valid fact names in the expert system)
  * @param timestamp timestamp for the fact
  * @param component component which this fact is providing info about e.g. ME+1/1/1, ME+1/1/1/DMB, etc.. (look up valid component names in the expert system component ontology)
  * @param severity severity level of the fact - please use the EXSYS_FACT_SEVERITY_* constants e.g. EXSYS_FACT_SEVERITY_INFO, EXSYS_FACT_SEVERITY_ERROR, etc..
  * @param description description of the fact
  * @param factParamNames names of the parameters of the fact i.e. elements that are specific to this fact type (look up specific fact definition in the expert system)
  * @param factParamValues values of the parameters supplied in the factParamNames parameter
  */
public void exsys_createFact(int factCollectionDocId, string type, time timestamp, string component, string severity, string description, 
                     dyn_string factParamNames, dyn_anytype factParamValues, dyn_string &exceptionInfo) {
  
  // get the fact collection node and create a new fact node in it
  int collectionNode = xmlFirstChild(factCollectionDocId, xmlFirstChild(factCollectionDocId));
  if (collectionNode == -1) {
    emu_addError("Exsys: exsys_createFact() could not find the factCollection root node in the given document ID = " + factCollectionDocId, exceptionInfo);
    return;
  }
  int factNode = _exsys_addXmlNode(factCollectionDocId, collectionNode, type, "", exceptionInfo);
  
  // construct the fact - base class (general) nodes
  _exsys_addXmlNode(factCollectionDocId, factNode, "time", exsys_formatTimeForXml(timestamp), exceptionInfo);
  _exsys_addXmlNode(factCollectionDocId, factNode, "component_id", component, exceptionInfo);
  _exsys_addXmlNode(factCollectionDocId, factNode, "severity", severity, exceptionInfo);
  _exsys_addXmlNode(factCollectionDocId, factNode, "descr", description, exceptionInfo);
  
  // construct the fact - this fact type specific nodes
  if (dynlen(factParamNames) != dynlen(factParamValues)) {
    emu_addError("Exsys: exsys_createFact(): dynlen(factParamNames) != dynlen(factParamValues)", exceptionInfo);
    return;
  }
  for (int i=1; i <= dynlen(factParamNames); i++) {
    _exsys_addXmlNode(factCollectionDocId, factNode, factParamNames[i], factParamValues[i], exceptionInfo);
  }
}

/** 
  * Adds a node to a given xml document and returns the node id.
  * @param docId document ID
  */
private int _exsys_addXmlNode(int docId, int parent, string name, string value = "", dyn_string &exceptionInfo) {
  int nodeId = xmlAppendChild(docId, parent, XML_ELEMENT_NODE, EXSYS_NAMESPACE_SHORT + ":" + name);
  if (nodeId == -1) {
    emu_addError("Exsys: _exsys_addXmlNode(): error while adding a child node \"" + name + "\" to XML document ID=" + docId + ", xmlAppendChild() returned -1", exceptionInfo);
    return;
  }
  if (parent == -1) {
    int retCode = xmlSetElementAttribute(docId, nodeId, "xmlns:" + EXSYS_NAMESPACE_SHORT, EXSYS_NAMESPACE);
    if (retCode == -1) { 
      emu_addError("Exsys: _exsys_addXmlNode(): error while setting namespace attribute to node \"" + name + "\" in XML document ID=" + docId + ", xmlSetElementAttribute() returned -1", exceptionInfo);
      return;
    }
  }
  if (strlen(value) > 0) {
    int retCode = xmlAppendChild(docId, nodeId, XML_TEXT_NODE, value);
    if (retCode == -1) { 
      emu_addError("Exsys: _exsys_addXmlNode(): error while adding value \"" + value + "\" to node \"" + name + "\" in XML document ID=" + docId + ", xmlAppendChild() returned -1", exceptionInfo);
      return;
    }
  }
  return nodeId;
}

/** Formats the given time t to XML format */
public string exsys_formatTimeForXml(time t) {
  return formatTime("%Y-%m-%dT%H:%M:%S", t);
}
