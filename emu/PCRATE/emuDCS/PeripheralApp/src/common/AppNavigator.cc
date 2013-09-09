#include "emu/pc/AppNavigator.h"

#include <string>
#include <sstream>

const std::string emu::pc::AppNavigator::noHRef = std::string();

emu::pc::AppNavigator& emu::pc::AppNavigator::addLink( const std::string& name, const std::string& href ){
  push_back( std::make_pair( name, href ) );
  return *this;
}

std::string emu::pc::AppNavigator::getHTML() const {
  std::stringstream ss;
  ss << "<span class=\"navbar\">" << std::endl;
  for ( std::size_t i = 0; i < size(); ++i ){
    if ( at( i ).second.size() > 0 ){
      ss << "<a href=\"" << at( i ).second << "\">" << at( i ).first << "</a>";
    }
    else{
      ss << "<span>" << at( i ).first << "</span>";
    }
    if ( i + 1 < size() ) ss << " | ";
  }
  ss << std::endl << "</span>";
  return ss.str();
}
