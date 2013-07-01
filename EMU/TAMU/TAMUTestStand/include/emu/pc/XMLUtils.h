#ifndef XMLUTILS_H_
#define XMLUTILS_H_
/*
 * XMLUtils.h
 *
 *  Created on: Jun 27, 2013
 *      Author: Austin Schneider
 */

#include <string>

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanNode.hpp>
#include <xalanc/XPath/NodeRefList.hpp>

namespace emu { namespace pc {

XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanNode)
XALAN_USING_XALAN(XalanDOMString)

XalanDocument * parseXMLString(const std::string &);
void dumpFileToString(const char *filename, std::string & output);
std::string domStringToString(XalanDOMString &);

}
} // namespaces


#endif /* XMLUTILS_H_ */
