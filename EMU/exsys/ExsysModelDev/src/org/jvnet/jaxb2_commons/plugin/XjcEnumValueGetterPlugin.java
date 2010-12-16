package org.jvnet.jaxb2_commons.plugin;

import com.sun.codemodel.JDefinedClass;
import com.sun.codemodel.JFieldVar;
import com.sun.codemodel.JMethod;
import com.sun.codemodel.JType;
import com.sun.tools.xjc.Options;
import com.sun.tools.xjc.Plugin;
import com.sun.tools.xjc.outline.EnumOutline;
import com.sun.tools.xjc.outline.Outline;
import java.util.Collections;
import java.util.List;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;

public class XjcEnumValueGetterPlugin extends Plugin {

    private static final String cuNs = "http://jaxb.dev.java.net/plugin/enumvaluegetter";

    @Override
    public String getOptionName() {
        return "XEnumValueGetter";
    }

    @Override
    public String getUsage() {
        return "  -XEnumValueGetter        :  generate enum value getter";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(cuNs);
    }

    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {
        for (EnumOutline en: outline.getEnums()) {
            JDefinedClass jclazz = en.clazz;
            
            JFieldVar valueField = null;
            for (JFieldVar jfield: jclazz.fields().values()) {
                if (jfield.name().equals("value")) {
                    valueField = jfield;
                    break;
                }
            }

            if (valueField != null && jclazz.getMethod("getValue", new JType[]{ }) == null) {
                JMethod jm = jclazz.method(1, String.class, "getValue");
                jm.body()._return(valueField);
            }

        }
        return true;
    }

}
