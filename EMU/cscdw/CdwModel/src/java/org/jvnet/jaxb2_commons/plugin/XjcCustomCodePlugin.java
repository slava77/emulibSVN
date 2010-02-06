/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jvnet.jaxb2_commons.plugin;

import com.sun.codemodel.JMethod;
import com.sun.tools.xjc.Options;
import com.sun.tools.xjc.Plugin;
import com.sun.tools.xjc.model.CPluginCustomization;
import com.sun.tools.xjc.outline.ClassOutline;
import com.sun.tools.xjc.outline.FieldOutline;
import com.sun.tools.xjc.outline.Outline;
import com.sun.tools.xjc.util.DOMUtils;
import java.util.Collections;
import java.util.List;
import javax.xml.namespace.QName;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;

/**
 *
 * @author valdo
 */
public class XjcCustomCodePlugin extends Plugin {

    private static final String ccNs = "http://hyperjaxb3.jvnet.org/ejb/schemas/customcode";
    private static final QName ccCode = new QName(ccNs, "code");

    @Override
    public String getOptionName() {
        return "Xcustomcode";
    }

    @Override
    public String getUsage() {
        return "  -Xcustomcode        :  enable custom code plugin";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(ccNs);
    }

    @Override
    public boolean isCustomizationTagName(String nsUri, String localName) {
        if (nsUri.equals(ccNs)) {
            if (localName.equals(ccCode.getLocalPart())) return true;
        }
        return false;
    }

    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (ClassOutline co : outline.getClasses()) {

            // Class level customization
            CPluginCustomization cc = co.target.getCustomizations().find(ccNs);
            if (cc != null) {
                cc.markAsAcknowledged();
                co.implClass.direct(DOMUtils.getElementText(cc.element));
            }

            // Method level customization
            for (FieldOutline fo: co.getDeclaredFields()) {
                CPluginCustomization fc = fo.getPropertyInfo().getCustomizations().find(ccNs);
                if (fc != null) {
                    fc.markAsAcknowledged();
                    String fname = fo.getPropertyInfo().displayName();
                    JMethod method = co.implClass.method(1, void.class, fname);
                }
            }
            
        }
        return true;
    }

}
