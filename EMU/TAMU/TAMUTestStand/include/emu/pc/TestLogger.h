/*
 * TestLogger.h
 *
 * Handles logging of tests, errors,and results
 * Behavior is such that logging occurs by default
 * However the end/begin logging methods are implemented for testing purposes
 *
 * Output of logger is formatted as XML
 *  Created on: Mar 5, 2013
 *      Author: Austin Schneider
 */

#ifndef TESTLOGGER_H_
#define TESTLOGGER_H_

// Emu includes
#include "emu/pc/BasicTable.h"

// system includes
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

namespace emu { namespace pc {

struct TestError {
  std::stringstream errorID;
  std::stringstream errorDescription;
  std::stringstream signalID;
};

const std::string DEFAULT_LOGGING_DIRECTORY = "log";

class TestLogger
{
public:
  TestLogger();
  TestLogger(std::string);
  void openFile(std::string);
  void closeFile();
  void startTest(std::string);
  void endTest(int);
  bool isTesting();
  void reportError(TestError &);
  void beginLogging();
  void endLogging();
  bool isLogging();
  void setBoard(std::string);
  void setWorkingDirectory(std::string);
  void setTester(std::string);

private:
  bool testing;
  bool logging;
  std::string tester;
  std::string currentBoard;
  std::string currentTest;
  std::string workingDirectory;
  std::fstream log;




};


} } //Namespaces


#endif /* TESTLOGGER_H_ */
