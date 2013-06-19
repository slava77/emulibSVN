/*
 * $Id: $
 */

// class header
#include "emu/pc/TMBTestManager.h"

// Emu includes
#include "emu/pc/CCBBackplaneTester.h"
#include "emu/pc/FirmwareTester.h"
#include "emu/pc/TMBExternalTester.h"

// system includes
#include <iostream>


namespace emu {  namespace pc {

using std::endl;
using std::cout;
using std::string;


TMBTestManager::TMBTestManager()
: sys_(0)
{}


void TMBTestManager::Init(ConfigurablePCrates * sys)
{
  sys_ = sys;

  // make sure the "current" crate in the system is set!
  if (sys_->crate() == 0)
  {
    cout<<__PRETTY_FUNCTION__<<" WARNING!!! The current crate is not set! Will not initialize the Test Manager!"<<endl;
    return;
  }

  // ****** register the tests here: ******

  RegisterTestGroup<CCBBackplaneTester>("CCBBackplaneTester");
  RegisterTestGroup<FirmwareTester>("FirmwareTester");
  //RegisterTestGroup<TMBExternalTester>("TMBExternalTester");

  std::vector< TMB * > tmbs = sys_->crate()->tmbs();

  for (size_t i = 0; i < tmbs.size(); i++)
  {
    testing_.push_back(false);
    logging_.push_back(false);
    boardLabel_.push_back("");
  }
}


std::vector<std::string> TMBTestManager::GetTestGroupLabels() const
{
  return testGroupLabels_;
}


boost::shared_ptr< TestWorkerBase > TMBTestManager::GetTester(const std::string& test_group, int tmb)
{
  if (tests_.find(test_group) == tests_.end())
  {
    return boost::shared_ptr< TestWorkerBase >(); // zero pointer
  }

  if (tmb < 0)
  {
    tmb = sys_->tmbN();
  }

  return tests_[test_group][tmb];
}


std::ostringstream & TMBTestManager::GetTestOutput(std::string test_group, int tmb)
{
  if (tmb < 0)
  {
    tmb = sys_->tmbN();
  }

  return testOutputs_[test_group][tmb];
}

void TMBTestManager::SetBoardLabel(std::string board, int tmb)
{
  boardLabel_[tmb] = board;
  testing_[tmb] = true;
  logging_[tmb] = true;
  std::map< std::string, std::vector<boost::shared_ptr<TestWorkerBase> > >::iterator i = tests_.begin();
  for(; i != tests_.end(); ++i)
  {
    i->second[tmb]->SetBoardLabel(board);
  }
}

std::string TMBTestManager::GetBoardLabel(int tmb)
{
  return boardLabel_[tmb];
}

bool TMBTestManager::IsLogging(int tmb)
{
  return logging_[tmb];
}

bool TMBTestManager::IsTesting(int tmb)
{
  return testing_[tmb];
}

void TMBTestManager::BeginLogging(int tmb)
{
  logging_[tmb] = true;
  std::map< std::string, std::vector<boost::shared_ptr<TestWorkerBase> > >::iterator i = tests_.begin();
  for(; i != tests_.end(); ++i)
  {
    i->second[tmb]->BeginLogging();
  }
}

void TMBTestManager::EndLogging(int tmb)
{
  logging_[tmb] = false;
  std::map< std::string, std::vector<boost::shared_ptr<TestWorkerBase> > >::iterator i = tests_.begin();
  for(; i != tests_.end(); ++i)
  {
    i->second[tmb]->EndLogging();
  }
}

void TMBTestManager::EndTesting(int tmb)
{
  testing_[tmb] = false;
  logging_[tmb] = false;
  std::map< std::string, std::vector<boost::shared_ptr<TestWorkerBase> > >::iterator i = tests_.begin();
  for(; i != tests_.end(); ++i)
  {
    i->second[tmb]->EndTesting();
  }
}


}}  // namespaces
