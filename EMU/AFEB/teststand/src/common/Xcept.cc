#include "AFEB/teststand/utils/Xcept.h"
#include <sstream>

string AFEB::teststand::utils::xhtmlformat_exception_history( xcept::Exception& e ){
  stringstream ss;
  ss << "<table class=\"Xcept\">";
  ss << "<tr><th colspan=\"3\">Exception stack</th></tr>";
  ss << "<tr><th>layer</th><th>message</th><th>raised at</th></tr>";

  std::vector<xcept::ExceptionInformation> & history = e.getHistory();
  std::vector<xcept::ExceptionInformation>::reverse_iterator i = history.rbegin();
  int iLayer = history.size();
  while ( i != history.rend() ){
    ss << "<tr>";
    ss << "<td>" << iLayer << "</td>";
    ss << "<td style=\"background-color: #dddd11;\"><![CDATA[" << i->getProperty("message") << "]]></td>";
    ss << "<td>" << i->getProperty("function") << " (" << i->getProperty("module") << ":" << i->getProperty("line") << ")</td>";
    ss << "</tr>";
    ++i;
    --iLayer;
  }

  ss << "</table>";

  return ss.str();
}
