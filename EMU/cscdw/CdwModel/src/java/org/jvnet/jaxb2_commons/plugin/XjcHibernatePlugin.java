/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jvnet.jaxb2_commons.plugin;

import com.sun.codemodel.JAnnotationUse;
import com.sun.codemodel.JClass;
import com.sun.tools.xjc.Options;
import com.sun.tools.xjc.Plugin;
import com.sun.tools.xjc.model.CPluginCustomization;
import com.sun.tools.xjc.outline.ClassOutline;
import com.sun.tools.xjc.outline.Outline;
import java.util.Collections;
import java.util.List;
import javax.xml.namespace.QName;
import org.hibernate.annotations.CacheConcurrencyStrategy;
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
public class XjcHibernatePlugin extends Plugin {

    private static final String hjNs = "http://hyperjaxb3.jvnet.org/ejb/schemas/customizations";
    private static final QName hjEntity = new QName(hjNs, "entity");

    private static final String jhpNs = "http://jaxb.dev.java.net/plugin/hibernate";
    private static final QName jhpCache = new QName(jhpNs, "cache");
    private static final String clsCache = "org.hibernate.annotations.Cache";

    @Override
    public String getOptionName() {
        return "Xhibernate";
    }

    @Override
    public String getUsage() {
        return "  -Xhibernate        :  enable generation of specific hibernate annotations";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(jhpNs);
    }

    @Override
    public boolean isCustomizationTagName(String nsUri, String localName) {
        if (nsUri.equals(jhpNs)) {
            if (localName.equals(jhpCache.getLocalPart())) return true;
        }
        return false;
    }

    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (ClassOutline classOutline : outline.getClasses()) {

            CPluginCustomization cEntity = CustomizationUtils.findCustomization(classOutline, hjEntity);
            if (cEntity == null) {
                continue;
            }

            // Process hibernate cache
            NodeList cacheNL = cEntity.element.getElementsByTagNameNS(jhpCache.getNamespaceURI(), jhpCache.getLocalPart());
            if (cacheNL.getLength() > 0) {
                String usageName = ((Element) cacheNL.item(0)).getAttribute("usage");
                if (usageName != null) {
                    JClass dc = outline.getCodeModel().directClass(clsCache);
                    JAnnotationUse annotation = classOutline.implClass.annotate(dc);
                    try {
                        annotation.param("usage", CacheConcurrencyStrategy.valueOf(usageName));
                    } catch (IllegalArgumentException e) {
                        errorHandler.fatalError(
                            new SAXParseException("Not valid hibernate cache usage value for class " + classOutline.target.fullName(),
                            cEntity.locator));
                    }
                    String regionName = ((Element) cacheNL.item(0)).getAttribute("region");
                    if (regionName != null) {
                        annotation.param("region", regionName);
                    }
                } else {
                    errorHandler.fatalError(
                        new SAXParseException("Hibernate cache usage attribute is missing for class " + classOutline.target.fullName(),
                        cEntity.locator));
                }
            }

        }
        return true;
    }

}
