#ifndef __AFEB_teststand_utils_DOM_h__
#define __AFEB_teststand_utils_DOM_h__

//#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <xalanc/XalanDOM/XalanNode.hpp>

#include "xcept/Exception.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace AFEB { namespace teststand { namespace utils {

      string serializeDOM( DOMNode* node );

      void setNodeValue( DOMNode* node, const string& value );

      string appendToSelectedNode( const string XML, const string xPathToNode, const string xmlFragment )
	throw( xcept::Exception );

      string setSelectedNodeValue( const string XML, const string xPathToNode, const string value )
	throw( xcept::Exception );

      string setSelectedNodesValues( const string XML, const map<string,string>& values )
	throw( xcept::Exception );

      string getSelectedNodeValue( const string& XML, const string xpath )
	throw( xcept::Exception );

      vector<string> getSelectedNodesValues( const string& XML, const string xpath )
	throw( xcept::Exception );

      XALAN_USING_XALAN(XalanNode)
      string getNodeValue( const XalanNode* const node );

    }
  }
}

#endif
