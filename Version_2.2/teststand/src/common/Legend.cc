#include "AFEB/teststand/Legend.h"

using namespace AFEB::teststand;

const int Legend::styles_[] = { Legend::solid, Legend::dotted, Legend::dashed, Legend::dottedDashed };
const int Legend::colors_[] = { Legend::black, Legend::red, Legend::blue, Legend::green  };

AFEB::teststand::Legend::Legend() :
  TLegend( 0.90, 0.1, 0.99, 0.9 ){
  SetHeader( "channel" );
  SetTextAlign( 22 ); // middle, middle
  SetTextSize( 0.025 );
  SetMargin( 0.6 ); // to make the line segment as long as 0.6 times the box width
}

int AFEB::teststand::Legend::getColor( const int i ) const {
  return ( i/nStyles_ < nColors_ ? colors_[ i/nStyles_ ] : 0 );
}

int AFEB::teststand::Legend::getStyle( const int i ) const {
  return styles_[ i%nStyles_ ];
}
