/*
 * $Id: $
 */

// class header
#include "emu/pc/TestResultsManager.h"

namespace emu { namespace pc {

using std::endl;
using std::cout;
using std::string;
using std::map;
using std::pair;
using std::multimap;
using std::vector;

void TestResultsManager::processDirectory(const std::string dir_name)
{
  if(TestResultsManagerCallTrace) cout << __func__ << " " << dir_name << endl;
  xm_.loadFiles(dir_name);
}

void TestResultsManager::refreshBoardLabels()
{
  if(TestResultsManagerCallTrace) cout << __func__ << endl;
  xm_.loadFiles(currentPath_);
  boardLabels_ = xm_.getNamesList();
}

void TestResultsManager::refreshTestLabels()
{
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::set<std::string>::iterator it = boardLabels_.begin();
  for(; it!=boardLabels_.end(); ++it)
  {
    XObjectPtr xptr = xm_.evaluate(*it, eval, "/root/log/test/@label");
    if(xptr.null()) continue;
    const NodeRefListBase & nodes = xptr->nodeset();
    for(size_t i=0, len = nodes.getLength(); i<len; ++i)
    {
      testLabels_.insert(XMLString::transcode(nodes.item(i)->getNodeValue().c_str()));
    }
  }
}

std::set<std::string> TestResultsManager::getBoardLabels()
{
  refreshBoardLabels();
  return boardLabels_;
}
std::set<std::string> TestResultsManager::getTestLabels()
{
  refreshTestLabels();
  return testLabels_;
}

std::vector<std::string> TestResultsManager::getTestTimes(std::string board, std::string test)
{
  std::vector<std::string> times;
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log/test[@label='" << test << "']/@time";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return times;
  const NodeRefListBase & nodes = xptr->nodeset();
  for(size_t i=0, len = nodes.getLength(); i<len; ++i)
  {
    times.push_back(XMLString::transcode(nodes.item(i)->getNodeValue().c_str()));
  }
  return times;
}

std::string TestResultsManager::getLatestTestTime(std::string board, std::string test)
{
  long long max_time = 0;
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log/test[@label='" << test << "']/@time";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return "";
  const NodeRefListBase & nodes = xptr->nodeset();
  for(size_t i=0, len = nodes.getLength(); i<len; ++i)
  {
    std::stringstream ss;
    ss << XMLString::transcode(nodes.item(i)->getNodeValue().c_str());
    long long time;
    ss >> time;
    if(time > max_time || max_time == 0)
    {
      max_time = time;
    }
  }
  std::stringstream ss;
  ss << max_time;
  return ss.str();
}

std::vector<std::string> TestResultsManager::getBoardLogTimes(std::string board)
{
  std::vector<std::string> times;
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log/@time";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return times;
  const NodeRefListBase & nodes = xptr->nodeset();
  for(size_t i=0, len = nodes.getLength(); i<len; ++i)
  {
    times.push_back(XMLString::transcode(nodes.item(i)->getNodeValue().c_str()));
  }
  return times;
}

std::string TestResultsManager::getLatestBoardLogTime(std::string board)
{
  long long max_time = 0;
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log/@time";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return "";
  const NodeRefListBase & nodes = xptr->nodeset();
  for(size_t i=0, len = nodes.getLength(); i<len; ++i)
  {
    std::stringstream ss;
    ss << XMLString::transcode(nodes.item(i)->getNodeValue().c_str());
    long long time;
    ss >> time;
    if(time > max_time || max_time == 0)
    {
      max_time = time;
    }
  }
  std::stringstream ss;
  ss << max_time;
  return ss.str();
}

std::vector<std::string> TestResultsManager::getLogTestTimes(std::string board, std::string log_time)
{
  std::vector<std::string> times;
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log" << "[@time='" << log_time << "']/test/@time";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return times;
  const NodeRefListBase & nodes = xptr->nodeset();
  for(size_t i=0, len = nodes.getLength(); i<len; ++i)
  {
    times.push_back(XMLString::transcode(nodes.item(i)->getNodeValue().c_str()));
  }
  return times;
}

std::string TestResultsManager::getTestName(std::string board, std::string test_time)
{
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log/test[@time='" << test_time << "']/@label";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return "";
  const NodeRefListBase & nodes = xptr->nodeset();
  if(nodes.getLength() > 0)
  {
    return XMLString::transcode(nodes.item(0)->getNodeValue().c_str());
  }
  else
    return "";
}

std::string TestResultsManager::getTestResult(std::string board, std::string test_time)
{
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr);
  XALAN_USING_XALAN(XalanDOMChar);
  XALAN_USING_XALAN(XPathEvaluator);
  XPathEvaluator eval;
  std::stringstream xpath;
  xpath << "/root/log/test[@time='" << test_time << "']/result/@value";
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
  if(xptr.null()) return "";
  const NodeRefListBase & nodes = xptr->nodeset();
  if(nodes.getLength() > 0)
  {
    return XMLString::transcode(nodes.item(0)->getNodeValue().c_str());
  }
  else
    return "";
}

std::vector<TestResultsManager::TestError> TestResultsManager::getTestErrors(std::string board, std::string test_time)
{
  std::vector<TestError> errors;
    XALAN_USING_XALAN(NodeRefListBase)
    XALAN_USING_XALAN(XObjectPtr);
    XALAN_USING_XALAN(XalanDOMChar);
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator eval;
    std::stringstream xpath;
    xpath << "/root/log/test[@time='" << test_time << "']/error";
    XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
    if(xptr.null()) return errors;
    const NodeRefListBase & nodes = xptr->nodeset();
    for(size_t i=0, len = nodes.getLength(); i<len; ++i)
    {
      errors.push_back(TestError());
      XalanDOMString attr;
      XalanNode * attribute;

      attr.assign("errorID");
      attribute = nodes.item(i)->getAttributes()->getNamedItem(attr);
      if(attribute != NULL) errors[i].errorID = XMLString::transcode(attribute->getNodeValue().c_str());

      attr.assign("description");
      attribute = nodes.item(i)->getAttributes()->getNamedItem(attr);
      if(attribute != NULL) errors[i].errorDescription = XMLString::transcode(attribute->getNodeValue().c_str());

      attr.assign("signalID");
      attribute = nodes.item(i)->getAttributes()->getNamedItem(attr);
      if(attribute != NULL) errors[i].signalID = XMLString::transcode(attribute->getNodeValue().c_str());
    }
    return errors;
}

}} // namespaces
