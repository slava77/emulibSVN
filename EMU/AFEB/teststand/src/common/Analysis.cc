#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/fit/Polynomial.h"
#include "AFEB/teststand/fit/LeastSquaresFitter.h"
#include "TFile.h"
#include "TH1D.h"
#include <iostream>
#include <sstream>
#include <set>

using namespace AFEB::teststand;

AFEB::teststand::Analysis::Analysis( const string& AFEB_ROOT, const string& resultsDir )
  : afebRootDir_       ( AFEB_ROOT                )
  , rawResultsDir_     ( resultsDir + "/raw"      )
  , analyzedResultsDir_( resultsDir + "/analyzed" ){
  string resultXML = utils::readFile( rawResultsDir_ + "/results.xml" );
  calibXML_        = utils::readFile( rawResultsDir_ + "/Calibration.xml" );
  configXML_       = utils::getSelectedNode( resultXML, "/root/c:configuration" );
  rawResultXML_    = utils::getSelectedNode( resultXML, "/root/a:results" );

  configuration_ = new Configuration( configXML_, "" );

  collectAnalyzedDevices();
}

AFEB::teststand::Analysis::~Analysis(){
  delete configuration_;
}

void AFEB::teststand::Analysis::analyze(){
  calibrateDACs();
  assignDACs();
  calculateGain();
  calculateInternalCapacitance();
  cout << analyzedDevices_ << endl;
}

void AFEB::teststand::Analysis::collectAnalyzedDevices(){

  // Get devices from configuration's first measurement
  if ( configuration_->getMeasurements().size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No measurements found!" );
  }

  // Collect all tested devices. By definition, each element of this set will be unique.
  set<TestedDevice*> allTestedDevices;
  for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ){
    // Get the devices tested in this measurement:
    set<TestedDevice*> td = (*m)->getTestedDevices();
    // Add them to the set of all devices:
    allTestedDevices.insert( td.begin(), td.end() );
  }

  // Construct an analyzed device out of every tested device:
  cout << "set<TestedDevice*>" << endl << allTestedDevices << endl;
  for ( set<TestedDevice*>::iterator tdi = allTestedDevices.begin(); tdi != allTestedDevices.end(); ++tdi ){
    analyzedDevices_.push_back( AnalyzedDevice( **tdi, rawResultsDir_, configuration_->getMeasurements() ) );
  }

  // Get the date and time of measurement
  stringstream xpath;
  xpath.str("/a:results/@measurementDate");
  string measurementDate = utils::getSelectedNodeValue( rawResultXML_, xpath.str() );

  // Associate with the analyzed devices the parameters of the adaptor used
  for ( vector<AnalyzedDevice>::iterator ad = analyzedDevices_.begin(); ad != analyzedDevices_.end(); ++ad ){

    ad->setMeasurementDate( measurementDate );

    // Get id and type of adaptor in use
    xpath.str("/c:configuration/c:inputs/@adaptorId");
    string adaptorId = utils::getSelectedNodeValue( configXML_, xpath.str() );
    ad->setAdaptorId( adaptorId );
    xpath.str("/c:configuration/c:inputs/@adaptorType");
    string adaptorType = utils::getSelectedNodeValue( configXML_, xpath.str() );
    ad->setAdaptorType( adaptorType );

    // Get the injection capacitance [pF] of this adaptor
    xpath.str("");
    xpath << "/c:calibrations/c:adaptors/c:adaptor[@id='" << adaptorId << "' and @type='" << adaptorType << "']/@injectionCapacitance";
    string capacitance = utils::getSelectedNodeValue( calibXML_, xpath.str() );
    // cout << xpath.str() << endl;
    // cout << capacitance << endl;
    if ( capacitance.size() == 0 ){
      stringstream ss; ss << "Failed to find the injection capacitance for adaptor " << adaptorId << " of type " << adaptorType;
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    ad->setInjectionCapacitance( utils::stringTo<double>( capacitance ) );

    // Get the pulse division factor of this adaptor
    xpath.str("");
    xpath << "/c:calibrations/c:adaptors/c:adaptor[@id='" << adaptorId << "' and @type='" << adaptorType << "']/@pulseDivisionFactor";
    string factor = utils::getSelectedNodeValue( calibXML_, xpath.str() );
    if ( capacitance.size() == 0 ){
      stringstream ss; ss << "Failed to find the pulse division factor for adaptor " << adaptorId << " of type " << adaptorType;
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    ad->setPulseDivisionFactor( utils::stringTo<double>( factor ) );

    // Get the correction coefficient for this adaptor's socket into which this device is plugged into
    xpath.str("");
    xpath << "/c:calibrations/c:adaptors/c:adaptor[@id='" << adaptorId << "' and @type='" << adaptorType 
	  << "']/c:socket[@number='" << ad->getAdaptorSocket() << "']/@coefficient";
    string coefficient = utils::getSelectedNodeValue( calibXML_, xpath.str() );
    // cout << xpath.str() << endl;
    // cout << coefficient << endl;
    if ( coefficient.size() == 0 ){
      stringstream ss; ss << "Failed to find the threshold correction coefficient for socket " << ad->getAdaptorSocket() << " of adaptor " << adaptorId << " of type " << adaptorType;
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    ad->setCorrectionCoefficient( utils::stringTo<double>( coefficient ) );
  }

  cout << "Analyzed devices" << endl << analyzedDevices_ << endl;
}

void AFEB::teststand::Analysis::calibrateDACs(){
  stringstream xpath;
  // Count DACs with calibration data
  size_t nDACs = utils::getSelectedNodesValues( calibXML_, "/c:calibrations/c:DACs/c:DAC" ).size();
  cout << "nDACs=" << nDACs << endl;
  for ( size_t iDAC=1; iDAC<=nDACs; ++iDAC ){
    //
    // Create DAC objects
    //
    xpath.str("");
    xpath << "/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@moduleId";
    string moduleId = utils::getSelectedNodeValue( calibXML_, xpath.str() );
    xpath.str("");
    xpath << "/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@moduleName";
    string moduleName = utils::getSelectedNodeValue( calibXML_, xpath.str() );
    xpath.str("");
    xpath << "/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@type";
    string type = utils::getSelectedNodeValue( calibXML_, xpath.str() );
    xpath.str("");
    xpath << "/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@channel";
    int channel = utils::stringTo<int>( utils::getSelectedNodeValue( calibXML_, xpath.str() ) );
    DACs_.push_back( DAC( moduleId, moduleName, type, channel ) );
    //
    // Load calibration data
    //
    xpath.str("");
    xpath << "/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/*[name()='c:value']/@DACUnits";
    vector< pair<string,string> > x = utils::getSelectedNodesValues( calibXML_, xpath.str() );
    cout << x << endl;
    xpath.str("");
    xpath << "/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/*[name()='c:value']/@milliVolts";
    vector< pair<string,string> > y = utils::getSelectedNodesValues( calibXML_, xpath.str() );
    cout << y << endl;
    //
    // Fit calibration data
    //
    if ( x.size() != y.size() ){
      stringstream ss;
      ss << "Calibration data size mismatch for " << DACs_.back();
      // DAC dac( moduleId, moduleName, type, channel );
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    fit::LeastSquaresFitter< fit::Polynomial<1> > p1fitter;
    vector< pair<string,string> >::const_iterator ix, iy;
    TMatrixD X(1,1);
    TMatrixD Y(1,1);
    TMatrixDSym V(1);
    V( 0, 0 ) = 1.; // [mV^2], the estimated variance of the measurement
    for ( ix = x.begin(), iy = y.begin(); ix != x.end() && iy != y.end(); ++ix, ++iy ){
      X( 0, 0 ) = utils::stringTo<double>( ix->second );
      Y( 0, 0 ) = utils::stringTo<double>( iy->second );
      p1fitter.addObservation( X, Y, V );
    }
    DACs_.back().setCalibrationParameters( p1fitter.getFittedParameters(),
					   p1fitter.getFittedParametersCovariance() );
    cout << *DACs_.back().getCalibrationParameters() << endl;
    cout << *DACs_.back().getCalibrationParametersCovariance() << endl;
    // TMatrixT<double> mm(2,2); mm(0,0)=-1.; mm(0,1)=2.; mm(1,0)=3.; mm(1,1)=4.;
    // cout << mm << endl;
  }
  cout << DACs_ << endl;
}

void AFEB::teststand::Analysis::assignDACs(){
  // Loop over the devices
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){

    // Find the pulse generator's and the threshold generator's DACs for this device
    for ( vector<DAC>::const_iterator dac=DACs_.begin(); dac!=DACs_.end(); ++dac ){
      if ( *dac == d->getPulseDACDescriptor() ){
	cout << "Found pulse DAC" << endl << *dac << endl;
	d->setPulseDAC( &*dac );
      }
      else if ( *dac == d->getThresholdDACDescriptor() ){
	cout << "Found threshold DAC" << endl << *dac << endl;
	d->setThresholdDAC( &*dac );
      }
    }

  }  
}

void AFEB::teststand::Analysis::calculateGain(){
  // Loop over the devices
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->calculateGains();
  }  
}


void AFEB::teststand::Analysis::calculateInternalCapacitance(){
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->calculateInternalCapacitances();
  }
}


void AFEB::teststand::Analysis::saveResults(){
  saveResults( analyzedResultsDir_ );
}

void AFEB::teststand::Analysis::saveResults( const string& destinationDir ){
  // Create directory for analyzed results
  utils::execShellCommand( string( "mkdir -p " ) + destinationDir );
  // Copy analyzed results' XSLT file to it
  stringstream command;
  command << "cp " << afebRootDir_ << "/AFEB/teststand/html/analyzedResults_XSLT.xml " << destinationDir;
  AFEB::teststand::utils::execShellCommand( command.str() );
  // Copy selection cuts' file to it
  command.str( "" );
  command << "cp " << afebRootDir_ << "/AFEB/teststand/xml/SelectionCuts.xml " << destinationDir;
  AFEB::teststand::utils::execShellCommand( command.str() );
  // Copy selection cuts' XSLT file to it. It is dummy, but the analyzed results' XSLT includes it, so it must be there.
  // It will be replaced with a real one if/when selections are applied.
  command.str( "" );
  command << "cp " << afebRootDir_ << "/AFEB/teststand/html/selectionCuts_XSLT.xml " << destinationDir;
  AFEB::teststand::utils::execShellCommand( command.str() );
  // Save each device's results
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->saveResults( afebRootDir_, destinationDir );
  }
}

void AFEB::teststand::Analysis::applySelection(){
  applySelection( analyzedResultsDir_ );
}

void AFEB::teststand::Analysis::applySelection( const string& analyzedResultsDirectory ){
  // Load selection cuts from file
  vector<Cut> cuts = loadSelectionCuts( analyzedResultsDirectory );
  // cout << cuts << endl;
  // Write XSLT for marking the selections in the results file.
  utils::writeFile( analyzedResultsDirectory + "/selectionCuts_XSLT.xml", createSelectionCutsXSLT( cuts ) );
  // Apply cuts to each device's results
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->passesSelectionCuts( analyzedResultsDirectory, cuts );
  }
}

vector<Cut> AFEB::teststand::Analysis::loadSelectionCuts( const string& dir ) const {
  vector<Cut> cuts;
  string cutsXML( utils::readFile( dir + "/SelectionCuts.xml" ) );
  vector< pair<string,string> > cutIds = utils::getSelectedNodesValues( cutsXML, "/s:selection/s:analyzedData/s:cut/@id" );
  // cout << "cut ids: " << cutIds << endl;
  for ( vector< pair<string,string> >::const_iterator ci=cutIds.begin(); ci!=cutIds.end(); ++ci ){
    cuts.push_back( utils::getSelectedNodesValues( cutsXML, string( "/s:selection/s:analyzedData/s:cut[@id='" ) + ci->second + "']/@*" ) );
  }
  return cuts;
}

string AFEB::teststand::Analysis::createSelectionCutsXSLT( const vector<Cut>& cuts ) const {
  stringstream ss;

  ss << "<?xml version=\"1.0\"?>" << endl;
  ss << "<xsl:transform xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" xmlns:ad=\"http://cms.cern.ch/emu/afeb/teststand/analyzeddevice\" xmlns:s=\"http://cms.cern.ch/emu/afeb/teststand/selection\" version=\"1.0\">" << endl;;
  for ( vector<Cut>::const_iterator c=cuts.begin(); c!=cuts.end(); ++c ){
    ss << "  <!-- cut" << c->getId() << " -->" << endl;
    ss << "  <xsl:template match=\"" << c->getXpath() << "\" mode=\"selection\">" << endl;
    ss << "    <xsl:param name=\"CUTID\" select=\"" << c->getId() << "\"/>" << endl;
    ss << "    <xsl:param name=\"VALUE\" select=\"translate(.,'+','')\"/>" << endl;
    ss << "    <xsl:param name=\"HIGH\" select=\"number(document('SelectionCuts.xml')/s:selection/s:analyzedData/s:cut[number(@id)=$CUTID]/@high)\"/>" << endl;
    ss << "    <xsl:param name=\"LOW\" select=\"number(document('SelectionCuts.xml')/s:selection/s:analyzedData/s:cut[number(@id)=$CUTID]/@low)\"/>" << endl;
    ss << "    <xsl:param name=\"RESULT\"><xsl:choose><xsl:when test=\"$HIGH > $VALUE and $VALUE > $LOW\">passed</xsl:when><xsl:otherwise>failed</xsl:otherwise></xsl:choose></xsl:param>" << endl;
    ss << "    <xsl:attribute name=\"title\"><xsl:apply-templates mode=\"descriptionText\" select=\".\"/> It <xsl:value-of select=\"$RESULT\"/> selection cut #<xsl:value-of select=\"$CUTID\"/>. Acceptable range: ( <xsl:value-of select=\"$LOW\"/> , <xsl:value-of select=\"$HIGH\"/> )</xsl:attribute>" << endl;
    ss << "    <xsl:attribute name=\"class\"><xsl:value-of select=\"$RESULT\"/></xsl:attribute>" << endl;
    ss << "  </xsl:template>" << endl;
  }
  ss << "</xsl:transform>";
  
  return ss.str();
}
