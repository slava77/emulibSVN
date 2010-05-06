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
public class XjcSuperClassPlugin extends Plugin {

    private static final String xjcNs = "http://java.sun.com/xml/ns/jaxb/xjc";
    private static final QName xjcSuperClass = new QName(xjcNs, "superClass");


    @Override
    public String getOptionName() {
        return "Xsuperclass";
    }

    @Override
    public String getUsage() {
        return "  -Xsuperclass        :  enable generation of specific super-classes";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(xjcNs);
    }

    @Override
    public boolean isCustomizationTagName(String nsUri, String localName) {
        if (nsUri.equals(xjcNs)) {
            if (localName.equals(xjcSuperClass.getLocalPart())) return true;
        }
        return false;
    }

    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (ClassOutline classOutline : outline.getClasses()) {

            CPluginCustomization cstSuperClass = CustomizationUtils.findCustomization(classOutline, xjcSuperClass);
            if (cstSuperClass != null) {
                Element elSC = cstSuperClass.element;
                String superClassName = elSC.getAttribute("name");
                if (superClassName != null) {
                    JClass dc = outline.getCodeModel().directClass(superClassName);
                    if (dc != null) {
                        classOutline.implClass._extends(dc);
                    } else {
                        errorHandler.fatalError(
                            new SAXParseException("Super class " + superClassName + " not defined/not found for class " + classOutline.target.fullName(),
                            cstSuperClass.locator));
                    }
                }
            }

        }
        return true;
    }

}
