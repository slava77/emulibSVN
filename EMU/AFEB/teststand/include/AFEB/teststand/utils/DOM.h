#ifndef __AFEB_teststand_utils_DOM_h__
#define __AFEB_teststand_utils_DOM_h__

#include <xercesc/dom/DOM.hpp>
//#include <xercesc/dom/DOMNode.hpp>

#include "xcept/Exception.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace AFEB { namespace teststand { namespace utils {

      string serializeDOM( DOMNode* node );

      void setNodeValue( DOMNode* node, const string& value );

    }
  }
}

#endif
