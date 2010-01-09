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
import javax.xml.namespace.QName;
import org.jvnet.jaxb2_commons.util.CustomizationUtils;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

/**
 *
 * @author valdo
 */
public class XjcSuperClassPlugin extends Plugin {

    private static final String hjNs = "http://hyperjaxb3.jvnet.org/ejb/schemas/customizations";
    private static final QName hjEntity = new QName(hjNs, "entity");

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
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (ClassOutline classOutline : outline.getClasses()) {

            CPluginCustomization cEntity = CustomizationUtils.findCustomization(classOutline, hjEntity);
            if (cEntity == null) {
                continue;
            }

            NodeList elSC = cEntity.element.getElementsByTagNameNS(xjcSuperClass.getNamespaceURI(), xjcSuperClass.getLocalPart());
            if (elSC.getLength() > 0) {
                String superClassName = ((Element) elSC.item(0)).getAttribute("name");
                if (superClassName != null) {
                    JClass dc = outline.getCodeModel().directClass(superClassName);
                    if (dc != null) {
                        classOutline.implClass._extends(dc);
                    } else {
                        errorHandler.fatalError(
                            new SAXParseException("Super class " + superClassName + " not defined/not found for class " + classOutline.target.fullName(),
                            cEntity.locator));
                    }
                }
            }

        }
        return true;
    }

}
