#include <iostream>
#include <string>
#include <sstream>
#include "EmuDisplayClient.h"

#include "xdaq.h"
#include "xgi/Method.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "pt/PeerTransportAgent.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"
#include "xcept/tools.h"


/*
  #include "TBuffer.h"
  #include "TMessage.h"
  #include "TObject.h"
  #include "TH1F.h"
  #include "TH1.h"
  #include "TCanvas.h"
  #include "THStack.h"
*/  
using namespace std;

XDAQ_INSTANTIATOR_IMPL(EmuDisplayClient)

  EmuDisplayClient::EmuDisplayClient(xdaq::ApplicationStub* stub)
  throw (xdaq::exception::Exception)
    : xdaq::WebApplication(stub),
      monitorClass_("EmuMonitor"),
      iconsURL_("http://cms-dqm03.phys.ufl.edu/dqm/results/")
{

  errorHandler_ = toolbox::exception::bind (this, &EmuDisplayClient::onError, "onError");

  fsm_.addState ('H', "Halted");
  fsm_.addState ('R', "Ready");
  fsm_.addState ('E', "Enabled");
  fsm_.addStateTransition ('H','R', "Configure", this, &EmuDisplayClient::ConfigureAction);
  fsm_.addStateTransition ('R','E', "Enable", this, &EmuDisplayClient::EnableAction);
  fsm_.addStateTransition ('E','H', "Halt", this, &EmuDisplayClient::HaltAction);
  fsm_.addStateTransition ('R','H', "Halt", this, &EmuDisplayClient::HaltAction);
  fsm_.setInitialState('H');
  fsm_.reset();

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &EmuDisplayClient::fireEvent, "Configure", XDAQ_NS_URI);
  xoap::bind (this, &EmuDisplayClient::fireEvent, "Enable", XDAQ_NS_URI);
  xoap::bind (this, &EmuDisplayClient::fireEvent, "Halt", XDAQ_NS_URI);

  // Bind CGI callbacks
  xgi::bind(this, &EmuDisplayClient::dispatch, "dispatch");
  xgi::bind(this, &EmuDisplayClient::getImagePage, "getImage");
  xgi::bind(this, &EmuDisplayClient::getEMUSystemViewPage, "getEMUSystemView");
  xgi::bind(this, &EmuDisplayClient::headerPage, "header");

  xgi::bind(this, &EmuDisplayClient::createTreePage, "tree.html");
  xgi::bind(this, &EmuDisplayClient::createTreeEngine, "tree.js");
  xgi::bind(this, &EmuDisplayClient::createTreeTemplate, "tree_tpl.js");
  xgi::bind(this, &EmuDisplayClient::createTreeItems, "tree_items.js");

  // Define Web state machine
  wsm_.addState('H', "Halted",    this, &EmuDisplayClient::stateMachinePage);
  wsm_.addState('R', "Ready",     this, &EmuDisplayClient::stateMachinePage);
  wsm_.addState('E', "Enabled",   this, &EmuDisplayClient::stateMachinePage);
  wsm_.addStateTransition('H','R', "Configure", this, &EmuDisplayClient::Configure, &EmuDisplayClient::failurePage);
  wsm_.addStateTransition('R','E', "Enable",    this, &EmuDisplayClient::Enable,    &EmuDisplayClient::failurePage);
  wsm_.addStateTransition('R','H', "Halt",      this, &EmuDisplayClient::Halt,      &EmuDisplayClient::failurePage);
  wsm_.addStateTransition('E','H', "Halt",      this, &EmuDisplayClient::Halt,      &EmuDisplayClient::failurePage);
  wsm_.setInitialState('H');

  xoap::bind (this, &EmuDisplayClient::updateList, "updateList", XDAQ_NS_URI);
  xoap::bind (this, &EmuDisplayClient::updateObjects, "updateObjects", XDAQ_NS_URI);

  getApplicationInfoSpace()->fireItemAvailable("monitorClass",&monitorClass_);
  getApplicationInfoSpace()->addItemChangedListener ("monitorClass", this);
  getApplicationInfoSpace()->fireItemAvailable("iconsURL",&iconsURL_);
  getApplicationInfoSpace()->addItemChangedListener ("iconsURL", this);

  // === Initialize ROOT system
  if (!gApplication)
    TApplication::CreateApplication();
 
  gStyle->SetPalette(1,0);
}
    
    
EmuDisplayClient::~EmuDisplayClient() 
{
}


//
// SOAP Callback trigger state change
//
xoap::MessageReference EmuDisplayClient::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode* node = body.getDOMNode();
  DOMNodeList* bodyList = node->getChildNodes();
  for (unsigned int i = 0; i < bodyList->getLength(); i++)
    {
      DOMNode* command = bodyList->item(i);

      if (command->getNodeType() == DOMNode::ELEMENT_NODE)
	{
	  std::string commandName = xoap::XMLCh2String (command->getLocalName());

	  try
	    {
	      toolbox::Event::Reference e(new toolbox::Event(commandName, this));
	      fsm_.fireEvent(e);
	      // Synchronize Web state machine
	      wsm_.setInitialState(fsm_.getCurrentState());
	    }
	  catch (toolbox::fsm::exception::Exception & e)
	    {
	      XCEPT_RETHROW(xcept::Exception, "invalid command", e);
	    }

	  xoap::MessageReference reply = xoap::createMessage();
	  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	  xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
	  envelope.getBody().addBodyElement ( responseName );
	  return reply;
	}
    }

  XCEPT_RAISE(xoap::exception::Exception,"command not found");
}

bool EmuDisplayClient::onError ( xcept::Exception& ex, void * context )
{
  LOG4CPLUS_INFO (getApplicationLogger(),"onError: " << ex.what());
  return false;
}

// == Run Control requests current parameter values == //
void EmuDisplayClient::actionPerformed (xdata::Event& e)
{
  if (e.type() == "ItemChangedEvent")
    {
      std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
      // std::cout << item << std::endl;

      if ( item == "monitorClass")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "monitor Class : " << monitorClass_.toString());
	  monitors_.clear();
	  monitors_ = getAppsList(monitorClass_);
        }
    }

}


void EmuDisplayClient::ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) 
{
    
  LOG4CPLUS_INFO (getApplicationLogger(),"Configure EmuDisplayClient for DQM Histos");
}

void EmuDisplayClient::EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Enable EmuDisplayClient");
}

void EmuDisplayClient::HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Stop EmuDisplayClient");
}

// XGI Call back

void EmuDisplayClient::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  //  wsm_.displayPage(out);
  createHTMLNavigation(in, out);
}

// WSM Dispatcher function
void EmuDisplayClient::dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
  std::string stateInput = (*stateInputElement).getValue();
  wsm_.fireEvent(stateInput,in,out);
}


string ageOfPageClock(){
  stringstream ss;
  ss << "<script type=\"text/javascript\">"                        << endl;
  ss << "   ageOfPage=0"                                           << endl;
  ss << "   function countSeconds(){"                              << endl;
  ss << "      hours=Math.floor(ageOfPage/3600)"                   << endl;
  ss << "      minutes=Math.floor(ageOfPage/60)%60"                << endl;
  ss << "      age=\"\""                                           << endl;
  ss << "      if (hours) age+=hours+\" h \""                      << endl;
  ss << "      if (minutes) age+=minutes+\" m \""                  << endl;
  ss << "      age+=ageOfPage%60+\" s \""                          << endl;
  ss << "      document.getElementById('ageOfPage').innerHTML=age" << endl;
  ss << "      ageOfPage=ageOfPage+1"                              << endl;
  ss << "      setTimeout('countSeconds()',1000)"                  << endl;
  ss << "   }"                                                     << endl;
  ss << "</script>"                                                << endl;
  return ss.str();
}

string autoResizeAction(int width, int height) {
  stringstream ss;
  ss << "<script type=\"text/javascript\">"
     << "window.onresize= alertSize;\n"
     << "function alertSize() {\n"
     << "var myWidth = " << width << ", myHeight = " << height << ";\n"
     << "if( typeof( window.innerWidth ) == \'number\' ) {\n"
     << "//Non-IE\n"
     << "myWidth = window.innerWidth-16;\n"
     << "myHeight = window.innerHeight-16;\n"
     << "} else if( document.documentElement && ( document.documentElement.clientWidth || document.documentElement.clientHeight ) ) {\n"
     << "//IE 6+ in 'standards compliant mode'\n"
     << "myWidth = document.documentElement.clientWidth-20;\n"
     << "myHeight = document.documentElement.clientHeight-20;\n"
     << "} else if( document.body && ( document.body.clientWidth || document.body.clientHeight ) ) {\n"
     << "//IE 4 compatible\n"
     << "myWidth = document.body.clientWidth-20;\n"
     << "myHeight = document.body.clientHeight-20;}\n"
     << "document.params.imageWidth.value = myWidth-20; "
     << "document.params.imageHeight.value = myHeight-100;"
     << "document.params.submit();}\n" 
     << "</script>\n";
  return ss.str();
}



int getCSCStatus(std::string cscname) {
  if (cscname.find("10") != std::string::npos) return 1;
  if (cscname.find("13") != std::string::npos) return 2;
  if (cscname.find("15") != std::string::npos) return 3;
  if (cscname.find("17") != std::string::npos) return 4;
  return 0;
}

std::string getCSCStatusLink(std::string cscname) {
  std::stringstream ss;
  std::string name = cscname.erase(0,cscname.rfind("/")+1);
  switch (getCSCStatus(cscname)) {
  case 0: // Not found
    ss << name;break;
  case 1: // Present
    ss << cgicc::a(name).set("href",""); break;
  default:
    ss << cgicc::a(name).set("href",""); break;        				      
  }
  return ss.str();
}

std::string getCSCStatusColor(std::string cscname) {
  
  switch (getCSCStatus(cscname)) {
  case 0: // Not found
    return "#d0d0d0";break;
  case 1: // Present
    return "#ffffff"; break;
  case 2: // Tests Ok
    return "#00ff00"; break; 
  case 3: // Warning
    return "#ffff00"; break;
  case 4: // Error
    return "#ff0000"; break;
  default:
    return "#d0d0d0";   				      
  }
}

string generateCSCTable() 
{
  std::stringstream ss;
  ss << cgicc::table().set("border","1")
    .set("cellspacing","0").set("cellpadding","0");
   
  std::vector<std::pair<string, int> > types;
  types.push_back(std::make_pair("ME+4/2",36));
  types.push_back(std::make_pair("ME+4/1",18));
  types.push_back(std::make_pair("ME+3/2",36));
  types.push_back(std::make_pair("ME+3/1",18));
   
  types.push_back(std::make_pair("ME+2/2",36));
  types.push_back(std::make_pair("ME+2/1",18));
  types.push_back(std::make_pair("ME+1/3",36));
  types.push_back(std::make_pair("ME+1/2",36));
  types.push_back(std::make_pair("ME+1/1",36));
  types.push_back(std::make_pair("ME-1/1",36));
  types.push_back(std::make_pair("ME-1/2",36));
  types.push_back(std::make_pair("ME-1/3",36));
  types.push_back(std::make_pair("ME-2/1",18));
  types.push_back(std::make_pair("ME-2/2",36));
  types.push_back(std::make_pair("ME-3/1",18));
  types.push_back(std::make_pair("ME-3/2",36));
  types.push_back(std::make_pair("ME-4/1",18));
  types.push_back(std::make_pair("ME-4/2",36));
   
  string cscname =  "";
  string csclink =  "";
  string cscbgcolor = "#d0d0d0";

  for (int i=0; i<9; i++) {
    ss << cgicc::tr();
    if (i==0) ss << cgicc::td("ME+").set("rowspan", "9").set("id","ME+").set("width","50").set("class","me") << "\n";
    ss << cgicc::td(types.at(i).first).set("id", types.at(i).first).set("width","80").set("class","me_type") << "\n";
    for (int j=1; j<=types.at(i).second; j++) { 
      cscname =  types.at(i).first+"/"+xdata::Integer(j).toString();
      csclink = getCSCStatusLink(cscname);
      cscbgcolor = getCSCStatusColor(cscname);
       
      // if (types.at(i).second ==18) {
      // ss << cgicc::td(csclink).set("id", cscname).set("colspan","2").set("bgcolor",cscbgcolor) << "\n";
      //} else {
      ss << cgicc::td(csclink).set("id", cscname).set("width","30").set("bgcolor",cscbgcolor) << "\n";
      // }
    }
    ss << cgicc::tr();
  }
  for (int i=9; i<18; i++) {
    ss << cgicc::tr();
    if (i==9) ss << cgicc::td("ME-").set("rowspan", "9").set("id","ME-").set("class","me") << "\n";
    ss << cgicc::td(types.at(i).first).set("id", types.at(i).first).set("class","me_type") << "\n";
    for (int j=1; j<=types.at(i).second; j++) {
      cscname =  types.at(i).first+"/"+xdata::Integer(j).toString();
      csclink = getCSCStatusLink(cscname);
      cscbgcolor = getCSCStatusColor(cscname);
      //if (types.at(i).second ==18) {
      //	 ss << cgicc::td(csclink).set("id",cscname).set("colspan","2").set("bgcolor",cscbgcolor) << "\n";
      //} else {
      ss << cgicc::td(csclink).set("id", cscname).set("bgcolor",cscbgcolor) << "\n";
      //}
    }
    ss << cgicc::tr();
  }
  ss << cgicc::table() << std::endl;
  return ss.str();
}

void EmuDisplayClient::getEMUSystemViewPage (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  int updateRate = 10;
  cgicc::Cgicc cgi(in);
  cgicc::const_form_iterator inputElement = cgi.getElement("updateRate");
  if (inputElement != cgi.getElements().end()) {
    updateRate = (*inputElement).getIntegerValue();
  }
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/getEMUSystemViewPage";

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::head();
  *out << "<meta http-equiv=\"refresh\" content=" << updateRate <<">" << std::endl;
  *out << cgicc::style() << cgicc::comment() << std::endl;
  *out << "td, tr{text-align: center; font-size:12; font-family:Arial; }";
  *out << "td.me, tr.me{ background-color: #ddd; text-align: center; font-size:16; font-family:Arial; }";
  *out << "td.me_type, tr.me_type{ background-color: #ddd; text-align: center; font-size:14; font-family:Arial; }";
  *out << cgicc::comment() << cgicc::style() << std::endl;
    
  *out << ageOfPageClock();
  *out << cgicc::title("EMU System View") << std::endl;
  *out << cgicc::head();
  *out << "<body onload=\"countSeconds()\">" << std::endl;
  *out << cgicc::form().set("name","params").set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;
  *out << "  Update Interval(sec):" <<  cgicc::input().set("type", "text").set("size","4").set("name", "updateRate").set("value",xdata::Integer(updateRate).toString());
  *out << cgicc::input().set("type", "submit").set("name", "setParams").set("value", "Update" );
  *out << cgicc::form()<< std::endl;

  *out << cgicc::h4("EMU System View - (Updated at " + xgi::Utils::getCurrentTime()+ "&#8212; <span id=\"ageOfPage\"></span> ago)").set("align","center")<< std::endl;

  *out << generateCSCTable() << std::endl;
  *out << cgicc::html();
}

void EmuDisplayClient::getImagePage (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string objectName = "";
  int nodeID = 0;
  std::string folderName = "";
  cgicc::const_form_iterator stateInputElement = cgi.getElement("objectName");
  if (stateInputElement != cgi.getElements().end()) {
    objectName = (*stateInputElement).getValue();
  }

  stateInputElement = cgi.getElement("folderName");
  if (stateInputElement != cgi.getElements().end()) {
    folderName = (*stateInputElement).getValue();
  }

  stateInputElement = cgi.getElement("nodeID");
  if (stateInputElement != cgi.getElements().end()) {
    nodeID = (*stateInputElement).getIntegerValue();
  } 
  // std::cout << stateInput << std::endl;
  // stateMachinePage(out );
  int w = 1024;
  int h = 768;
  int updateRate = 10;
  //  std::string autoUpdate = "checked";
  bool autoUpdate = cgi.queryCheckbox("autoUpdate");
  bool imgSize = false;

  stateInputElement = cgi.getElement("imageWidth");
  if (stateInputElement != cgi.getElements().end()) {
    w = (*stateInputElement).getIntegerValue();
    imgSize=true;
  }
  stateInputElement = cgi.getElement("imageHeight");
  if (stateInputElement != cgi.getElements().end()) {
    h = (*stateInputElement).getIntegerValue();
    imgSize=true;
  }

  stateInputElement = cgi.getElement("updateRate");
  if (stateInputElement != cgi.getElements().end()) {
    updateRate = (*stateInputElement).getIntegerValue();
  }
  
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::head();
  if (autoUpdate)
    *out << "<meta http-equiv=\"refresh\" content=" << updateRate <<">" << std::endl;
  *out << autoResizeAction(w, h);
  *out << ageOfPageClock();
  *out << cgicc::title("Show Object - /node"+(xdata::Integer(nodeID)).toString() + "/"+ folderName + "/" + objectName) << std::endl;
  *out << cgicc::head();
  std::string imgname = "node"+(xdata::Integer(nodeID)).toString() + "_" +folderName + "_" + objectName+".png";
  std::string imgurl = "";
  imgurl += getApplicationDescriptor()->getContextDescriptor()->getURL();
  // imgurl += "/emu/emuDQM/emuDisplayClient/images/"+imgname;
  imgurl += "/tmp/images/"+imgname;
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/getImage";

  *out << "<body onload=\"countSeconds()\">" << std::endl;
  *out << cgicc::form().set("name","params").set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;
  *out << "Image Width:" << cgicc::input().set("type", "text").set("size","4").set("name", "imageWidth").set("value", xdata::Integer(w).toString() ).set("onchange","textchanged()");
  *out << "Height:" <<  cgicc::input().set("type", "text").set("size","4").set("name", "imageHeight").set("value", xdata::Integer(h).toString()).set("onchange","textchanged()") 
       << std::endl;
  // *out << "AutoUpdate:" <<  cgicc::input().set("type", "checkbox").set("name", "autoUpdate");
  *out << "  Update Interval(sec):" <<  cgicc::input().set("type", "text").set("size","4").set("name", "updateRate").set("value",xdata::Integer(updateRate).toString());
  if (autoUpdate)
    *out << "Auto:" <<  cgicc::input().set("type", "checkbox").set("name", "autoUpdate").set("checked", "checked" );
  else
    *out << "Auto:" <<  cgicc::input().set("type", "checkbox").set("name", "autoUpdate") ;
   
  *out << cgicc::input().set("type", "hidden").set("name", "objectName").set("value", objectName );
  *out << cgicc::input().set("type", "hidden").set("name", "folderName").set("value", folderName );
  *out << cgicc::input().set("type", "hidden").set("name", "nodeID").set("value", (xdata::Integer(nodeID)).toString() );
  
  *out << cgicc::input().set("type", "submit").set("name", "setParams").set("value", "Set/Update" );
  *out << cgicc::form()<< std::endl;

  //   *out << cgicc::h2(objectName) << std::endl;
  *out << cgicc::h4("Object: \"/node"+(xdata::Integer(nodeID)).toString() + "/"+ folderName + "/" + objectName + "\""
		    + " (Updated at " + xgi::Utils::getCurrentTime()+ "&#8212; <span id=\"ageOfPage\"></span> ago)")<< std::endl;
  //  *out << "&#8212; <span id=\"ageOfPage\"></span> ago " << std::endl;
  /*
    TH1* histo = new TH1F(objectName.c_str(),objectName.c_str(),100,0,100);
    ConsumerCanvas* canvas = new ConsumerCanvas(objectName.c_str(), objectName.c_str(),objectName.c_str(),1,1, atoi(w.c_str()), atoi(h.c_str()));
    canvas->cd(1);
    histo->Draw();
    canvas->Print(string("images/"+imgname).c_str());
    *out << cgicc::img().set("src", imgurl).set("alt",imgname) << std::endl;
    delete canvas;
    delete histo;
  */
  // TMessage* msgbuf = requestObjects(nodeID, folderName, objectName);
 
  xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeID);
  std::string state =  emu::dqm::getScalarParam(getApplicationContext(), d,"stateName","string"); 
  if (state=="") {
    *out << "<h3>ERROR: " << d->getClassName() <<"-" << d->getInstance() << " node is not available</h3>" << std::endl;
//    *out << cgicc::h3("ERROR: Object is not available right now.") << std::endl;
    *out << cgicc::html();
    return;
  } 
  TMessage* msgbuf = requestCanvas(nodeID, folderName, objectName,w, h);
  if (msgbuf != NULL) {
    msgbuf->Reset();
    msgbuf->SetReadMode();
    if (msgbuf->What() == kMESS_OBJECT) {
      TObject* obj = msgbuf->ReadObject(msgbuf->GetClass());
      if (obj->InheritsFrom(TH1::Class())) {
	TCanvas* canvas = new TCanvas(objectName.c_str(), objectName.c_str(), w, h);
	canvas->cd();
	((TH1*)obj)->Draw();    
	canvas->Print(string("/tmp/images/"+imgname).c_str());
        // if (!imgSize) {
	//   *out << cgicc::img().set("src", imgurl).set("alt",imgname).set("width","100%").set("height","95%") << std::endl;
        //} else {
	*out << cgicc::img().set("src", imgurl).set("alt",imgname) << std::endl;
        // }
	delete canvas;
      }
      if (obj->InheritsFrom(TCanvas::Class())) {
	((TCanvas*)obj)->SetCanvasSize(w, h);
	((TCanvas*)obj)->Print(string("/tmp/images/"+imgname).c_str());
        //if (!imgSize) {
	//  *out << cgicc::img().set("src", imgurl).set("alt",imgname).set("width","100%").set("height","95%") << std::endl;
	//} else {
	*out << cgicc::img().set("src", imgurl).set("alt",imgname) << std::endl;
        //}	
	// *out << cgicc::img().set("src", imgurl).set("width", w).set("height", h).set("alt",imgname) << std::endl;

      }
    }
    delete msgbuf;
  } else {
    *out << cgicc::h3("ERROR: Object is not available right now.") << std::endl;
  }

  
  *out << cgicc::html();
  
}

void EmuDisplayClient::headerPage (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  int updateRate = 10;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::head() << std::endl;
  *out << "<meta http-equiv=\"refresh\" content=" << updateRate <<">" << std::endl;
  *out << "<style>\n"
       << "th{text-align: center; font-size:12px; font-family:Arial; Tahoma, Verdana }"
       << "td, tr{text-align: right; font-size:12px; font-family:Arial; Tahoma, Verdana }"
       << "body\n"
       << "        {color: #000000; text-decoration: none; font-family: Arial, Tahoma, Verdana; font-size: 12px}\n"
       << "a, A:link, a:visited, a:active, A:hover\n"
       << "        { text-decoration: none; font-family: Arial, Tahoma, Verdana; font-size: 12px}\n"
       << "</style>\n" << std::endl;
  *out << cgicc::head() << std::endl;
  *out << cgicc::body();
  /*
    xgi::Utils::getPageHeader
    (out,
    "EmuDisplayClient",
    getApplicationDescriptor()->getContextDescriptor()->getURL(),
    getApplicationDescriptor()->getURN(),
    "/daq/xgi/images/Application.gif"
    );
  */
  
  monitors_ = getAppsList(monitorClass_);
  *out << cgicc::table().set("border","1")
    .set("cellspacing","0").set("cellpadding","0");
  *out << cgicc::th("Node").set("width","80") << cgicc::th("State") << cgicc::th("Run Number")
       << cgicc::th("DAQ Events").set("width","60") 
       << cgicc::th("DQM Events").set("width","60") << cgicc::th("Rate (Evt/s)").set("width","60") 
       << cgicc::th("Unpacked CSCs").set("width","60") <<  cgicc::th("Unpack Rate (CSCs/s)").set("width","80") 
       << cgicc::th("Readout Mode").set("width","60") << cgicc::th("Data Source").set("width","60")
       << cgicc::th("Last event timestamp")
    /* << cgicc::th("Bandwidth").set("width","60")  << cgicc::th("Latency").set("width","60") */
       <<  std::endl;
  if (!monitors_.empty()) {
    //    *out << cgicc::b("Monitors List:")<<cgicc::br() << std::endl;    
    for (int i=0; i<monitors_.size(); i++) {

      std::ostringstream st;
      st << monitors_[i]->getClassName() << "-" << monitors_[i]->getInstance();
      std::string applink = monitors_[i]->getContextDescriptor()->getURL()+"/"+monitors_[i]->getURN();
      std::string state =  emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"stateName","string");
      
      if (state == "") { 
	state = "Unknown/Dead";
	*out << cgicc::tr();
        *out << cgicc::td("<a href="+applink+" target=frameset>"+st.str()+"</a>" )
	     << cgicc::td(state) << std::endl;
        *out << cgicc::tr() << std::endl;
	continue;
      }
      else {
        std::string stateChangeTime = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"stateChangeTime","string");
        state += " at " + stateChangeTime;
      }

      
      std::string runNumber   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"runNumber","unsignedLong");
      std::string events = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"sessionEvents","unsignedLong");
      std::string dataRate   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"dataRate","string");
      std::string cscUnpacked   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"cscUnpacked","unsignedLong");
      std::string cscRate   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"cscRate","string");
      std::string readoutMode   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"readoutMode","string");
      std::string lastEventTime = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"lastEventTime","string");
      
      std::string nDAQevents = "";
      std::string dataSource = "";
      if (readoutMode == "internal")
	dataSource   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"inputDeviceName","string");
      if (readoutMode == "external") {
        dataSource   = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"serversClassName","string");
	nDAQevents = emu::dqm::getScalarParam(getApplicationContext(), monitors_[i],"nDAQEvents","unsignedLong");
      }
      
      *out << cgicc::tr();
      *out << cgicc::td("<a href="+applink+" target=frameset>"+st.str()+"</a>" )
	   << cgicc::td(state) << cgicc::td(runNumber) << cgicc::td(nDAQevents) <<cgicc::td(events) << cgicc::td(dataRate) 
	   << cgicc::td(cscUnpacked) << cgicc::td(cscRate) << cgicc::td(readoutMode) << cgicc::td(dataSource) 
	   << cgicc::td(lastEventTime)/* << cgicc::td(dataBw) << cgicc::td(dataLatency) */ <<std::endl;
      *out << cgicc::tr() << std::endl;
    }
  }
  *out << cgicc::table();

  *out << cgicc::body() << cgicc::html();
  
}


//
// Web Events that trigger state changes (result of wsm::fireEvent)
//
void EmuDisplayClient::Configure(xgi::Input * in ) throw (xgi::exception::Exception)
{
  try
    {
      toolbox::Event::Reference e(new toolbox::Event("Configure", this));
      fsm_.fireEvent(e);
    }
  catch (toolbox::fsm::exception::Exception & e)
    {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    }
}

void EmuDisplayClient::Enable(xgi::Input * in ) throw (xgi::exception::Exception)
{
  try
    {
      toolbox::Event::Reference e(new toolbox::Event("Enable", this));
      fsm_.fireEvent(e);
    }
  catch (toolbox::fsm::exception::Exception & e)
    {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    }
}

void EmuDisplayClient::Halt(xgi::Input * in ) throw (xgi::exception::Exception)
{
  try
    {
      toolbox::Event::Reference e(new toolbox::Event("Halt", this));
      fsm_.fireEvent(e);
    }
  catch (toolbox::fsm::exception::Exception & e)
    {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    }
}

//
// Web Navigation Pages
//
void EmuDisplayClient::stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuDisplayClient",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";

  /*
  // display FSM
  std::set<std::string> possibleInputs = wsm_.getInputs(wsm_.getCurrentState());
  std::set<std::string> allInputs = wsm_.getInputs();


  *out << cgicc::h3("Finite State Machine").set("style", "font-family: arial") << std::endl;
  *out << "<table border cellpadding=10 cellspacing=0>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<th>" << wsm_.getStateName(wsm_.getCurrentState()) << "</th>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  std::set<std::string>::iterator i;
  for ( i = allInputs.begin(); i != allInputs.end(); i++)
  {
  *out << "<td>";
  *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;

  if ( possibleInputs.find(*i) != possibleInputs.end() )
  {
  *out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
  }
  else
  {
  *out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled", "true");
  }

  *out << cgicc::form();
  *out << "</td>" << std::endl;
  }
  *out << "</tr>" << std::endl;
  *out << "</table>" << std::endl;
  //

  *out << cgicc::hr() << std::endl;
  */
  /*
    std::string measurementURL = "/";
    measurementURL += getApplicationDescriptor()->getURN();
    measurementURL += "/displayMeasurements";

    *out << cgicc::br() << "<a href=\"" << measurementURL << "\">" << "Display Measurements" << "</a>" << cgicc::br() << endl;
    */

  //   xgi::Utils::getPageFooter(*out);
}

//
// Failure Pages
//
void EmuDisplayClient::failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception)
{
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuDisplayClient Failure",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  *out << cgicc::br() << e.what() << cgicc::br() << endl;
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  *out << cgicc::br() << "<a href=\"" << url << "\">" << "retry" << "</a>" << cgicc::br() << endl;

  xgi::Utils::getPageFooter(*out);
}

xoap::MessageReference EmuDisplayClient::updateList(xoap::MessageReference node) throw (xoap::exception::Exception)
{
  xoap::SOAPBody rb = node->getSOAPPart().getEnvelope().getBody();
  if (rb.hasFault() )
    {
      xoap::SOAPFault fault = rb.getFault();
      string errmsg = "DQMNode: ";
      errmsg += fault.getFaultString();
      XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } else {                            
    }
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPBody b = envelope.getBody();
  xoap::SOAPName replyName = envelope.createName("updateListReply", "", "");
  xoap::SOAPBodyElement e = b.addBodyElement ( replyName );
  xoap::SOAPName statusName = envelope.createName("Status", "", "");
  xoap::SOAPElement statusElement = e.addChildElement(statusName);
  statusElement.addTextNode("LIST_UPDATED");
  return reply;
}

std::map<std::string, std::list<std::string> > EmuDisplayClient::requestObjectsList(xdaq::ApplicationDescriptor* monitor)
{

  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestObjectsList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPName originator ("originator", "", "");
  //  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  xoap::SOAPElement command = body.addBodyElement(commandName );
    
  std::map<std::string, std::list<std::string> > bmap;
  bmap.clear();
  // Get reply from DQM node and populate TConsumerInfo list
  try
    {
      LOG4CPLUS_INFO (getApplicationLogger(), "Sending requestObjectsList to " << monitor->getClassName() << " ID" << monitor->getLocalId());
      // xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, monitor);

      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	} else {       
	  LOG4CPLUS_INFO (getApplicationLogger(), "Received requestObjectsList reply from " << monitor->getClassName() << " ID" << monitor->getLocalId());
	  //	  std::map<std::string, std::list<std::string> > bmap;
	  std::list<std::string> olist;
	  vector<xoap::SOAPElement> content = rb.getChildElements ();
	  xoap::SOAPName nodeTag ("DQMNode", "", "");
	  for (vector<xoap::SOAPElement>::iterator itr = content.begin();
	       itr != content.end(); ++itr) {
            
	    std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
	    for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
		 n_itr != nodeElement.end(); ++n_itr) {
	      stringstream stdir;
	      bmap.clear();
	      xoap::SOAPName branchTag ("Branch", "", "");

	      std::vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );

	      for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin();
		   b_itr != branchElement.end(); ++b_itr) {

		olist.clear();
		stringstream stdir;
		string dir="";
		stdir.clear();
		dir = b_itr->getValue();
		// dir = "MonitorNode"+n_itr->getValue()+ "/" + dir;
		xoap::SOAPName objTag("Obj", "", "");

		vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );

		for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
		     o_itr != objElement.end(); ++o_itr ) {
		  olist.push_back(o_itr->getValue());
		  // Problem with duplicates search
		
		  string value = o_itr->getValue();
		  /*
		    int pos = value.rfind("/",value.size());
		    if (pos != string::npos) {
		    string name = value.substr(pos+1,value.size());
		    string path = dir+"/"+value.substr(0, pos);
		    cout << "path:" << path << " name:" << name << endl;
		    }
		  */
		  // std::cout << dir << "/" << value << std:: endl;
		  olist.sort();

		}
		bmap[b_itr->getValue()] = olist;
	      }
	      //	      objlist[n_itr->getValue()] = bmap;
	    }
	  }
	}
    }
  catch (xdaq::exception::Exception& e)
    {
      return bmap;
      // handle exception
    }

  LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Objects List is updated");
  return bmap;
}

std::map<std::string, std::list<std::string> > EmuDisplayClient::requestCanvasesList(xdaq::ApplicationDescriptor* monitor)
{

  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestCanvasesList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPName originator ("originator", "", "");
  //  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  xoap::SOAPElement command = body.addBodyElement(commandName );
    
  std::map<std::string, std::list<std::string> > bmap;
  bmap.clear();
  // Get reply from DQM node and populate TConsumerInfo list
  try
    {
      LOG4CPLUS_INFO (getApplicationLogger(), "Sending requestCanvasesList to " << monitor->getClassName() << " ID" << monitor->getLocalId());
      // xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, monitor);

      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	} else {       
	  LOG4CPLUS_INFO (getApplicationLogger(), "Received requestCanvasesList reply from " << monitor->getClassName() << " ID" << monitor->getLocalId());
	  //	  std::map<std::string, std::list<std::string> > bmap;
	  std::list<std::string> olist;
	  vector<xoap::SOAPElement> content = rb.getChildElements ();
	  xoap::SOAPName nodeTag ("DQMNode", "", "");
	  for (vector<xoap::SOAPElement>::iterator itr = content.begin();
	       itr != content.end(); ++itr) {
            
	    std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
	    for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
		 n_itr != nodeElement.end(); ++n_itr) {
	      stringstream stdir;
	      bmap.clear();
	      xoap::SOAPName branchTag ("Branch", "", "");

	      std::vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );

	      for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin();
		   b_itr != branchElement.end(); ++b_itr) {

		olist.clear();
		stringstream stdir;
		string dir="";
		stdir.clear();
		dir = b_itr->getValue();
		// dir = "MonitorNode"+n_itr->getValue()+ "/" + dir;
		xoap::SOAPName objTag("Obj", "", "");

		vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );

		for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
		     o_itr != objElement.end(); ++o_itr ) {
		  olist.push_back(o_itr->getValue());
		  string value = o_itr->getValue();
		  olist.sort();

		}
		bmap[b_itr->getValue()] = olist;
	      }
	
	    }
	  }
	}
    }
  catch (xdaq::exception::Exception& e)
    {
      return bmap;
      // handle exception
    }

  LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Canvases List is updated");
  return bmap;
}

xoap::MessageReference EmuDisplayClient::updateObjects(xoap::MessageReference node) throw (xoap::exception::Exception)
{
  xoap::SOAPName cmdTag ("updateObjects", "", "");
  vector<xoap::SOAPElement> content = node->getSOAPPart().getEnvelope().getBody().getChildElements (cmdTag);
  xoap::SOAPName originator ("originator", "", "");
  xdata::Integer nodeAddr(0);
  for (vector<xoap::SOAPElement>::iterator itr = content.begin(); itr != content.end();++itr) 
    {        
      nodeAddr.fromString(itr->getAttributeValue(originator));
    }    
  list<xoap::AttachmentPart*> attachments = node->getAttachments();
  list<xoap::AttachmentPart*>::iterator iter;
  /*
    for (iter = attachments.begin(); iter != attachments.end(); iter++) 
    {
    int size = (*iter)->getSize();
    char * content = (*iter)->getContent();

    TMessage* buf = new TMessage(kMESS_OBJECT);
    buf->Reset();
    buf->WriteBuf(content, size);  
    buf->Reset();
    // MIME Headers functions does not return correct values
    map<string, string, less<string> > mimeHdrs = (*iter)->getAllMimeHeaders();       
    string objname="DQMNode"+nodeAddr.toString()+"/" +(*iter)->getContentLocation();
    MessageStorage *storage = new MessageStorage(objname.c_str());
    buf->Reset();
    storage->updateMessage(buf);
    userServer->addStorage(storage);    //writes info to stdout
    }  
  */                            
  LOG4CPLUS_INFO (getApplicationLogger(), "+++ Objects are updated");
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPBody b = envelope.getBody();
  xoap::SOAPName replyName = envelope.createName("updateObjectsReply", "", "");
  xoap::SOAPBodyElement e = b.addBodyElement ( replyName );
  xoap::SOAPName statusName = envelope.createName("UpdateStatus", "", "");
  xoap::SOAPElement statusElement = e.addChildElement(statusName);
  statusElement.addTextNode("OBJECTS_UPDATED");

  return reply;
}

TMessage* EmuDisplayClient::requestObjects(xdata::Integer nodeaddr, std::string folder, std::string objname)
{
  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestObjects","xdaq", "urn:xdaq-soap:3.0");


  xoap::SOAPElement command = body.addBodyElement(commandName );

  xoap::SOAPName folderName = envelope.createName("Folder", "", "");
  xoap::SOAPElement folderElement = command.addChildElement(folderName);
  folderElement.addTextNode(folder);

  xoap::SOAPName objectName = envelope.createName("Object", "", "");
  xoap::SOAPElement objectElement = folderElement.addChildElement(objectName);
  objectElement.addTextNode(objname);
    
  // Get reply from DQM node and populate TConsumerInfo list
  TMessage* buf = NULL;
  try
    {
      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      LOG4CPLUS_INFO (getApplicationLogger(), "Sending requestObjects to " << d->getClassName() << " ID" << d->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, d);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	} else { 
          LOG4CPLUS_INFO (getApplicationLogger(), "Received requestObjects reply from " << d->getClassName() << " ID" << d->getLocalId());	  
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  std::list<xoap::AttachmentPart*>::iterator iter;
	  if (attachments.size() == 0) LOG4CPLUS_WARN (getApplicationLogger(), "Received empty object " << folder << "/" << objname);
	  
	  for (iter = attachments.begin(); iter != attachments.end(); iter++)
	    {
	      int size = (*iter)->getSize();
	      char * content = (*iter)->getContent();

	      buf = new TMessage(kMESS_OBJECT);
	      buf->Reset();
	      buf->WriteBuf(content, size);
	      buf->Reset();
	      
	      map<string, string, less<string> > mimeHdrs = (*iter)->getAllMimeHeaders();
              std:: cout << "size:" << size << " ,name: " << (*iter)->getContentLocation() << std::endl;
	      // std:: cout << (*iter)->getContentLocation() << std::endl;
	      
	      // buf->Reset();
	      
	    }
	}
    } 
  catch (xdaq::exception::Exception& e)
    {
      return buf;
      // handle exception
    }	
  
  LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Object is updated");
  return buf;
}

TMessage* EmuDisplayClient::requestCanvas(xdata::Integer nodeaddr, std::string folder, std::string objname, int width, int height)
{
  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestCanvas","xdaq", "urn:xdaq-soap:3.0");


  xoap::SOAPElement command = body.addBodyElement(commandName );

  xoap::SOAPName folderName = envelope.createName("Folder", "", "");
  xoap::SOAPElement folderElement = command.addChildElement(folderName);
  folderElement.addTextNode(folder);

  xoap::SOAPName objectName = envelope.createName("Canvas", "", "");
  xoap::SOAPElement objectElement = folderElement.addChildElement(objectName);
  objectElement.addTextNode(objname);

  xoap::SOAPName widthTag = envelope.createName("Width", "", "");
  objectElement.addAttribute(widthTag, xdata::Integer(width).toString());
  xoap::SOAPName heightTag = envelope.createName("Height", "", "");
  objectElement.addAttribute(heightTag, xdata::Integer(height).toString());

    
  // Get reply from DQM node and populate TConsumerInfo list
  TMessage* buf = NULL;
  try
    {
      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      LOG4CPLUS_INFO (getApplicationLogger(), "Sending requestCanvas: \"" << folder << "/" << objname << "\" to " << d->getClassName() << " ID" << d->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, d);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      /*
	std::cout << std::endl;
	reply->writeTo(cout);
	std::cout << endl;
      */

      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	} else { 
          LOG4CPLUS_INFO (getApplicationLogger(), "Received requestCanvas: \"" << folder << "/" << objname << "\" reply from " << d->getClassName() << " ID" << d->getLocalId());
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  std::list<xoap::AttachmentPart*>::iterator iter;
	  if (attachments.size() == 0) LOG4CPLUS_WARN (getApplicationLogger(), "Received empty object " << folder << "/" << objname);

	  for (iter = attachments.begin(); iter != attachments.end(); iter++)
	    {
	      int size = (*iter)->getSize();
	      char * content = (*iter)->getContent();
	      if (buf != NULL) {
		delete buf;
	      }
	      buf = new TMessage(kMESS_OBJECT);
	      buf->Reset();
	      buf->WriteBuf(content, size);

	    }
	}
    } 
  catch (xdaq::exception::Exception& e)
    {
      return buf;
      // handle exception
    }	
  
  // LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Canvas is updated");
  return buf;
}

// == Get Application Descriptors for specified Data Server class name == //
std::vector<xdaq::ApplicationDescriptor*> EmuDisplayClient::getAppsList(xdata::String className)
{
  std::vector<xdaq::ApplicationDescriptor*> applist;
  try
    {
      applist.clear();
      applist = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
      sort(applist.begin(), applist.end(), Compare_ApplicationDescriptors());
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_ERROR (getApplicationLogger(),
		       "No Applications with class name " << className.toString() <<
		       "found." << xcept::stdformat_exception_history(e));
    }
  return applist;
}
