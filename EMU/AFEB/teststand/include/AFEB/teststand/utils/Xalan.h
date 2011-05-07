#ifndef __AFEB_teststand_utils_Xalan_h__
#define __AFEB_teststand_utils_Xalan_h__

#include "xcept/Exception.h"

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XPath/NodeRefList.hpp>

using namespace std;

namespace AFEB { namespace teststand { namespace utils {

  int transformStreams( istream& source, istream& stylesheet, ostream& target )
    throw( xcept::Exception );

  int transformWithParams( istream& source, istream& stylesheet, ostream& target, const map<std::string,std::string>& params )
    throw( xcept::Exception );

  XALAN_USING_XALAN(XalanDocument)

  string serialize( const XalanDocument* document )
    throw( xcept::Exception );

  XALAN_USING_XALAN(XalanNode)

  XalanNode* getSingleNode( XalanDocument* doc, XalanNode* contextNode, const string xPath )
    throw( xcept::Exception );

  XALAN_USING_XALAN(NodeRefList)

  void getNodes( NodeRefList& resultNodeList, XalanDocument* document, XalanNode* contextNode, const string xPath )
    throw( xcept::Exception );

}}}

#endif
