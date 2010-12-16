/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jvnet.jaxb2_commons.plugin;

import com.sun.codemodel.JClass;
import com.sun.codemodel.JDefinedClass;
import com.sun.tools.xjc.Options;
import com.sun.tools.xjc.Plugin;
import com.sun.tools.xjc.model.CCustomizations;
import com.sun.tools.xjc.model.CPluginCustomization;
import com.sun.tools.xjc.outline.ClassOutline;
import com.sun.tools.xjc.outline.EnumOutline;
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
public class XjcClassUtilsPlugin extends Plugin {

    private static final String cuNs = "http://jaxb.dev.java.net/plugin/classutils";
    private static final QName cuExtends = new QName(cuNs, "extends");
    private static final QName cuImplements = new QName(cuNs, "implements");


    @Override
    public String getOptionName() {
        return "XClassUtils";
    }

    @Override
    public String getUsage() {
        return "  -XClassUtils        :  enable generation of specific class stuff";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(cuNs);
    }

    @Override
    public boolean isCustomizationTagName(String nsUri, String localName) {
        if (nsUri.equals(cuNs)) {
            if (localName.equals(cuExtends.getLocalPart())) return true;
            if (localName.equals(cuImplements.getLocalPart())) return true;
        }
        return false;
    }

    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (ClassOutline classOutline : outline.getClasses()) {
            CCustomizations custs = CustomizationUtils.getCustomizations(classOutline);
            applyExtends(outline, classOutline.implClass, custs, errorHandler);
            applyImplements(outline, classOutline.implClass, custs, errorHandler);
        }

        for (EnumOutline enumOutline : outline.getEnums()) {
            CCustomizations custs = CustomizationUtils.getCustomizations(enumOutline);
            applyExtends(outline, enumOutline.clazz, custs, errorHandler);
            applyImplements(outline, enumOutline.clazz, custs, errorHandler);
        }

        return true;
    }

    private boolean applyExtends(Outline outline, JDefinedClass jclass, CCustomizations customizations, ErrorHandler errorHandler) throws SAXException {
        CPluginCustomization cs = customizations.find(cuNs, cuExtends.getLocalPart());
        if (cs != null) {
            Element elExtends = cs.element;
            String className = elExtends.getAttribute("name");
            if (className != null) {
                JClass dc = outline.getCodeModel().directClass(className);
                if (dc != null) {
                    jclass._extends(dc);
                    cs.markAsAcknowledged();
                    return true;
                } else {
                    errorHandler.fatalError(
                        new SAXParseException("Super class " + className + " not defined/not found for class " + jclass.fullName(),
                        cs.locator));
                }
            }
        }
        return false;
    }

    private boolean applyImplements(Outline outline, JDefinedClass jclass, CCustomizations customizations, ErrorHandler errorHandler) throws SAXException {
        CPluginCustomization cs = customizations.find(cuNs, cuImplements.getLocalPart());
        if (cs != null) {
            Element elExtends = cs.element;
            String className = elExtends.getAttribute("name");
            if (className != null) {
                JClass dc = outline.getCodeModel().directClass(className);
                if (dc != null) {
                    jclass._implements(dc);
                    cs.markAsAcknowledged();
                    return true;
                } else {
                    errorHandler.fatalError(
                        new SAXParseException("Interface class " + className + " not defined/not found for class " + jclass.fullName(),
                        cs.locator));
                }
            }
        }
        return false;
    }

}
