#include "AFEB/teststand/Cut.h"

#include "AFEB/teststand/utils/String.h"

AFEB::teststand::Cut::Cut( const vector< pair<string,string> >& parameters ){
  for ( vector< pair<string,string> >::const_iterator p=parameters.begin(); p!=parameters.end(); ++p ){
    if      ( p->first == "id"          ) id_          = p->second;
    else if ( p->first == "low"         ) low_         = utils::stringTo<double>( p->second );
    else if ( p->first == "high"        ) high_        = utils::stringTo<double>( p->second );
    else if ( p->first == "description" ) description_ = p->second;
    else if ( p->first == "xpath"       ) xpath_       = p->second;
  }
}

ostream& AFEB::teststand::operator<<( ostream& os, const Cut& c ){
  os << "Cut id "       << c.id_
     << "    ( "        << c.low_
     << " , "           << c.high_
     << " )  \""        << c.description_
     << "\"   XPath=\"" << c.xpath_
     << "\"";
  return os;
}
