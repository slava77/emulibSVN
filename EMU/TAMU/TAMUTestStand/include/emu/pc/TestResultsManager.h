/*
 * TestResultsManager.h
 *
 *  Created on: Jul 26, 2012
 *      Author: Austin Schneider
 */

#ifndef TESTRESULTSMANAGER_H_
#define TESTRESULTSMANAGER_H_


//System Includes
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <time.h>
#include <stdio.h>

//Emu Includes
#include "emu/utils/String.h"
#include "emu/utils/DOM.h"
#include "emu/utils/Xalan.h"
#include "emu/pc/BasicTable.h"
#include "emu/pc/TestUtils.h"
#include "emu/pc/XMLUtils.h"
#include "emu/pc/XMLWrapper.h"

//Library Includes
#include <boost/regex.hpp>
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"

#include "xercesc/dom/DOMXPathResult.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/MemBufFormatTarget.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/SAXParseException.hpp"

#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/XPath/XObject.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/XPath/NodeRefList.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp"
#include "xalanc/XSLT/XSLTInputSource.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeInit.hpp"

namespace emu { namespace pc {

const unsigned int LINE_BUFFER_LENGTH = 256;
const char JUNK_ID = '0';
const char DATA_ID = '1';
const std::string DEFAULT_LOG_PATH = "log";

struct Key {
  std::string regEx;
  std::string identifiers;
};

struct testingInstance {
  int testResult;
  std::multimap<std::string, std::pair<int, int> > value_comparisons;
};

typedef std::map<std::time_t, testingInstance> Test;

typedef std::map<std::string, Test> Board;

class TestResultsManager
{
  static const bool TestResultsManagerCallTrace = false;

public:

  typedef BasicTable<std::string>::row_iterator row_iterator;

  // Default Constructor
  TestResultsManager()
  {
    initializeKeys();
    currentPath_ = DEFAULT_LOG_PATH;
    resultsTable_.addColumn("BoardLabel");
    resultsTable_.addColumn("TestLabel");
    resultsTable_.addColumn("TestTime");
    resultsTable_.addColumn("TestResult");
    resultsTable_.addColumn("ErrorCodes");
  };

  // Load logs on construction
  TestResultsManager(const std::string dir_name)
  {
    TestResultsManager();
    processDirectory(dir_name);
  };

  // Process a log file for test results
  void processFile(const std::string f_name);

  // Process a directory of log files for test results
  void processDirectory(const std::string dir_name);

  std::string getCurrentPath()
  {
    return currentPath_;
  }

  std::set<std::string> getBoardLabels();
  std::set<std::string> getTestLabels();
  BasicTable<std::string>::row_iterator getBegin(std::string column_label, std::string query, row_iterator begin, row_iterator end);
  BasicTable<std::string>::row_iterator getEnd(std::string column_label, std::string query, row_iterator begin, row_iterator end);
  BasicTable<std::string>::row_iterator begin();
  BasicTable<std::string>::row_iterator end();

  void setBoardLabel(std::string);
  std::string getBoardLabel();
  void setTestLabel(std::string);
  std::string getTestLabel();
  void setTime(time_t);
  void setTime(std::string);
  std::string getTime();

private:
  std::fstream file_;

  // the last directory path to be set
  std::string currentPath_;

  // files already loaded (used to prevent duplicate entries)
  std::set<std::string> loadedFiles_;

  std::set<std::string> keyLabels_;
  std::map<std::string, Key> keys_;

  BasicTable<std::string> resultsTable_;

  std::set<std::string> boardLabels_;

  std::set<std::string> testLabels_;

  // the last board label to be set
  std::string boardLabel_;

  // the last test label to be set
  std::string testLabel_;

  // the last time to be set
  std::string time_;

  /*
   * Creates keys and their subKeys
   */
  void initializeKeys();

  /*
   * Opens file with fstream
   */
  void openFile(const std::string);

  /*
   * process a single line from file_
   */
  void processLine();

  void processXML(std::string);

  /*
   *
   */
  void insertTestResult(std::string & boardLabel, std::string & testLabel, std::string & time, std::string test_result);

  /*
   *
   */
  void sortTable();
};

} } //namespaces



#endif /* TESTRESULTSMANAGER_H_ */
