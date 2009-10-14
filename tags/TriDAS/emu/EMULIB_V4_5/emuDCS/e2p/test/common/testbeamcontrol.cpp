//-----------------------------------------------------------------------
// $Id: testbeamcontrol.cpp,v 1.1 2008/02/14 17:11:38 geurts Exp $
// $Log: testbeamcontrol.cpp,v $
// Revision 1.1  2008/02/14 17:11:38  geurts
// additional new additions for the xdaqr4 update
//
// Revision 2.1  2005/06/15 13:54:46  geurts
// Changed default run behavior.
// testbeamcontrol runs a full Configure and Enable. Use -i option for
// interactive use, including the possibility to execute Disable
//
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//-----------------------------------------------------------------------
#include <iostream>
#include "VMEController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "TestBeamCrateController.h"
#include "TMB.h"
#include "ALCTController.h"
#include "MPC.h"
#include <unistd.h>
#include "PeripheralCrateParser.h"
#include "EMUParser.h"

int main(int argc, char **argv)
{
  char *xmlFile = "config.xml";
  bool doInteractive(false);

  // process command line arguments
  if (argc>1)
    for (int i=1;i<argc;i++){
      if (!strcmp(argv[i],"-h")){                                                                     
        std::cout << "Usage: " << argv[0] <<"[-f (file="<< xmlFile <<")]" << std::endl;
        exit(0);
      }
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
      if (!strcmp(argv[i],"-i")) doInteractive=true;
    }

 
  std::cout << "PeripheralCrate configuration file: " << xmlFile << std::endl;
  //PeripheralCrateParser parser;
  EMUParser parser;
  parser.parseFile(xmlFile);

  TestBeamCrateController tbController;
  std::cout << "Created TestBeamController" << std::endl;  
 

 
  std::cout << "Configuring TestBeamController: " << std::endl; 
  tbController.configure_simple();
  //tbController.configureNoDCS();

  std::cout << "TestBeamCrateController configured." << std::endl;
  //exit(0);
   while(1);

  // Examples of direct access to some modules 
  //DAQMB * daqmb = parser.daqmbParser().daqmb();
  //CCB * ccb = parser.ccbParser().ccb();
  //DAQMBTester tester(daqmb, ccb, false);
  //tester.runAllTests();


  // wait for the user to hit return-key before disabling
  if (doInteractive){
    std::cout << "Hit Return to continue ..." << std::endl;
    char wait[10];
    std::cin.getline(wait,1);
  }

  std::cout << "Enabling TestBeamCrateController" << std::endl;
  tbController.enable();
  std::cout << "TestBeamCrateController enabled." << std::endl;

  // wait for the user to hit return-key before disabling
  if (doInteractive){
    std::cout << "Hit Return to continue ..." << std::endl;
    char wait[10];
    std::cin.getline(wait,1);
  
    // only do a disable in the interactive mode
    tbController.disable();
    std::cout<< "Finished TestBeamCrateController disable" << std::endl;
  }
  
  return 0;
}

