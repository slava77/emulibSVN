/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jvnet.jaxb2_commons.plugin;

import com.sun.codemodel.JClass;
import com.sun.tools.xjc.Options;
import com.sun.tools.xjc.Plugin;
import com.sun.tools.xjc.model.CPluginCustomization;
import com.sun.tools.xjc.outline.ClassOutline;
import com.sun.tools.xjc.outline.Outline;
import java.util.Collections;
import java.util.List;
import javax.xml.namespace.QName;
import org.jvnet.jaxb2_commons.util.CustomizationUtils;
import org.w3c.dom.Element;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

/**
 *
 * @author valdo
 */
public class XjcIgnoreOnlyThisPlugin extends Plugin {

    private static final String xjcNs = "http://java.sun.com/xml/ns/jaxb/xjc";
    private static final QName xjcIgnoreOnlyThis = new QName(xjcNs, "ignoreOnlyThis");


    @Override
    public String getOptionName() {
        return "Xignoreonlythis";
    }

    @Override
    public String getUsage() {
        return "  -Xignoreonlythis        :  enable ignoring classes as entities but without affecting the subclasses";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(xjcNs);
    }

    @Override
    public boolean isCustomizationTagName(String nsUri, String localName) {
        if (nsUri.equals(xjcNs)) {
            if (localName.equals(xjcIgnoreOnlyThis.getLocalPart())) return true;
        }
        return false;
    }

    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (ClassOutline classOutline : outline.getClasses()) {
            CPluginCustomization cstIgnoreOnlyThis = CustomizationUtils.findCustomization(classOutline, xjcIgnoreOnlyThis);
            if (cstIgnoreOnlyThis != null) {
                System.out.println("Found ignoreOnlyThis customization tag");
//                Element elSC = cstSuperClass.element;
//                String superClassName = elSC.getAttribute("name");
//                if (superClassName != null) {
//                    JClass dc = outline.getCodeModel().directClass(superClassName);
//                    if (dc != null) {
//                        classOutline.implClass._extends(dc);
//                    } else {
//                        errorHandler.fatalError(
//                            new SAXParseException("Super class " + superClassName + " not defined/not found for class " + classOutline.target.fullName(),
//                            cstSuperClass.locator));
//                    }
//                }
            }

        }
        return true;
    }

}
