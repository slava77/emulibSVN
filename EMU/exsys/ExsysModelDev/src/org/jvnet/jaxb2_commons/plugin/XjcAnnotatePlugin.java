/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jvnet.jaxb2_commons.plugin;

import com.sun.codemodel.JAnnotationArrayMember;
import com.sun.codemodel.JAnnotationUse;
import com.sun.codemodel.JClass;
import com.sun.codemodel.JCodeModel;
import com.sun.codemodel.JDefinedClass;
import com.sun.codemodel.JFieldVar;
import com.sun.codemodel.JMethod;
import com.sun.codemodel.JType;
import com.sun.tools.xjc.Options;
import com.sun.tools.xjc.Plugin;
import com.sun.tools.xjc.model.CPluginCustomization;
import com.sun.tools.xjc.outline.ClassOutline;
import com.sun.tools.xjc.outline.EnumOutline;
import com.sun.tools.xjc.outline.FieldOutline;
import com.sun.tools.xjc.outline.Outline;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.xml.namespace.QName;
import org.jvnet.jaxb2_commons.util.CustomizationUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

/**
 *
 * @author valdo
 */
public class XjcAnnotatePlugin extends Plugin {

    private static final String annoNS = "http://jaxb.dev.java.net/plugin/annotate";
    private static final QName  annoDefaultQN = new QName(annoNS, "annotate");
    private static final QName  annoGetQN = new QName(annoNS, "annotateGet");
    private static final QName  annoSetQN = new QName(annoNS, "annotateSet");

    @Override
    public String getOptionName() {
        return "XAnnotate";
    }

    @Override
    public String getUsage() {
        return "  -XAnnotate        :  enable generation of specific project annotations";
    }

    @Override
    public List<String> getCustomizationURIs() {
        return Collections.singletonList(annoDefaultQN.getNamespaceURI());
    }

    @Override
    public boolean isCustomizationTagName(String nsUri, String localName) {
        return nsUri.equals(annoDefaultQN.getNamespaceURI()) && 
                (localName.equals(annoDefaultQN.getLocalPart()) ||
                 localName.equals(annoGetQN.getLocalPart()) ||
                 localName.equals(annoSetQN.getLocalPart()));
    }

    @SuppressWarnings("unchecked")
    @Override
    public boolean run(Outline outline, Options options, ErrorHandler errorHandler) throws SAXException {

        for (ClassOutline classOutline : outline.getClasses()) {
            
            CPluginCustomization cs = null;
            
            try {

                // Annotate class
                cs = CustomizationUtils.findCustomization(classOutline, annoDefaultQN);
                if (cs != null) {
                    List<AnnotationData> data = getAnnotation(outline, cs, errorHandler);
                    for (AnnotationData annData: data) {
                        JAnnotationUse ann = classOutline.implClass.annotate(annData.getJClass());
                        annData.applyParams(ann);
                    }
                    cs.markAsAcknowledged();
                }

                // Annotate field
                for (FieldOutline fieldOutline: classOutline.getDeclaredFields()) {
                    String name = fieldOutline.getPropertyInfo().getName(false);
                    cs = fieldOutline.getPropertyInfo().getCustomizations().find(annoDefaultQN.getNamespaceURI(), annoDefaultQN.getLocalPart());
                    if (cs != null) {
                        List<AnnotationData> data = getAnnotation(outline, cs, errorHandler);
                        for (AnnotationData annData: data) {
                            JFieldVar f = classOutline.implClass.fields().get(name);
                            JAnnotationUse ann = f.annotate(annData.getJClass());
                            annData.applyParams(ann);
                        }
                        cs.markAsAcknowledged();
                    }
                }

                // Annotate getter
                for (FieldOutline fieldOutline: classOutline.getDeclaredFields()) {
                    String name = fieldOutline.getPropertyInfo().getName(true);
                    cs = fieldOutline.getPropertyInfo().getCustomizations().find(annoGetQN.getNamespaceURI(), annoGetQN.getLocalPart());
                    if (cs != null) {
                        List<AnnotationData> data = getAnnotation(outline, cs, errorHandler);
                        JType rawType = fieldOutline.getRawType();

                        boolean isItem = false;

                        if (rawType instanceof JDefinedClass) {
                            for (EnumOutline eo: outline.getEnums()) {
                                if (rawType.fullName().equals(eo.clazz.fullName())) {
                                    isItem = true;
                                    break;
                                }
                            }
                        } else {
                            if (rawType.fullName().equals("javax.xml.datatype.XMLGregorianCalendar")) {
                                isItem = true;
                            }
                        }

                        JMethod m = null;

                        if (isItem) {
                            m = classOutline.implClass.getMethod("get" + name + "Item", new JType[0]);
                        }

                        if (m == null) {
                            m = classOutline.implClass.getMethod("get" + name, new JType[0]);
                        }

                        for (AnnotationData annData: data) {
                            
                            if (m != null) {
                                JAnnotationUse ann = m.annotate(annData.getJClass());
                                annData.applyParams(ann);
                            }
                        }
                        cs.markAsAcknowledged();
                    }
                }

                // Annotate setter
                for (FieldOutline fieldOutline: classOutline.getDeclaredFields()) {
                    String name = fieldOutline.getPropertyInfo().getName(true);
                    cs = fieldOutline.getPropertyInfo().getCustomizations().find(annoSetQN.getNamespaceURI(), annoSetQN.getLocalPart());
                    if (cs != null) {
                        List<AnnotationData> data = getAnnotation(outline, cs, errorHandler);
                        for (AnnotationData annData: data) {
                            JType [] mtypes = new JType[1];
                            mtypes[0] = fieldOutline.getRawType();
                            JMethod m = classOutline.implClass.getMethod("set" + name, mtypes );
                            if (m != null) {
                                JAnnotationUse ann = m.annotate(annData.getJClass());
                                annData.applyParams(ann);
                            }
                        }
                        cs.markAsAcknowledged();
                    }
                }

            } catch (Exception e) {
                e.printStackTrace(System.err);
                errorHandler.fatalError(
                    new SAXParseException(classOutline.target.fullName() + ": " + e.toString(), (cs != null ? cs.locator : null))
                );
            }
        }
        return true;
    }

    private List<AnnotationData> getAnnotation(Outline outline, CPluginCustomization cs, ErrorHandler errorHandler) throws SAXException, ClassNotFoundException, NoSuchMethodException {
        List<AnnotationData> data = new ArrayList<AnnotationData>();
        NodeList list = ((Node) cs.element).getChildNodes();
        for (int i = 0; i < list.getLength(); i++) {
            Node cNode = list.item(i);
            if (cNode.getNodeType() == Node.ELEMENT_NODE) {
                data.add(new AnnotationData(outline, (Element) cNode));
            }
        }
        return data;
    }

    private class AnnotationData {

        private JClass  jclazz;
        private List<AnnotationParamData> params = new ArrayList<AnnotationParamData>();

        @SuppressWarnings("unchecked")
        public AnnotationData(Outline outline, Element el) throws ClassNotFoundException, NoSuchMethodException {
            String className = el.getNamespaceURI() + "." + el.getLocalName();
            jclazz = outline.getCodeModel().directClass(className);
            Class clazz = this.getClass().getClassLoader().loadClass(className);
            if (!clazz.isInterface()) throw new IllegalArgumentException("Not valid annotation class (not interface!)");
            for (int j = 0; j < el.getAttributes().getLength(); j++) {
                Node attr = el.getAttributes().item(j);
                String pname = attr.getLocalName();
                String pvalue = attr.getNodeValue();
                Method m = clazz.getDeclaredMethod(pname);
                AnnotationParamData paramData = new AnnotationParamData(pname, pvalue, m.getReturnType());
                params.add(paramData);
            }

        }

        public JClass getJClass() {
            return jclazz;
        }

        public void applyParams(JAnnotationUse ann) throws ClassNotFoundException {
            for (AnnotationParamData param: params) {
                param.apply(ann);
            }
        }

    }

    private class AnnotationParamData {

        private String name;
        private String value;
        private Class type;

        public AnnotationParamData(String name, String value, Class type) {
            this.name = name;
            this.value = value;
            this.type = type;
        }

        @SuppressWarnings("unchecked")
        public void apply(JAnnotationUse ann) throws ClassNotFoundException {

            // Array
            if (type.isArray()) {

                JAnnotationArrayMember arrayMember = ann.paramArray(name);
                for (String avalue: this.value.split(",")) {
                    AnnotationParamData p = new AnnotationParamData(this.name, avalue, this.type.getComponentType());
                    p.apply(arrayMember);
                }
                return;

            // Simple type
            } else

            // Enum
            if (type.isEnum()) {
                
                ann.param(name, Enum.valueOf(type, value));
                return;

            } else

            // String
            if (type.equals(String.class)) {
                
                ann.param(name, value);
                return;

            } else

            // Class
            if (type.equals(Class.class)) {
                
                ann.param(name, this.getClass().getClassLoader().loadClass(value));
                return;

            } else

            // Primitives
            if (type.isPrimitive()) {

                // int
                if (type.getSimpleName().equals("int")) {

                    ann.param(name, Integer.parseInt(value));
                    return;

                } else

                // boolean
                if (type.getSimpleName().equals("boolean")) {

                    ann.param(name, Boolean.parseBoolean(value));
                    return;

                }
            }

            throw new ClassNotFoundException("Parameter type " + type.toString() + " is not supported by plugin.");

        }

        @SuppressWarnings("unchecked")
        private void apply(JAnnotationArrayMember arr) throws ClassNotFoundException {

            // Enum
            if (type.isEnum()) {

                //arr.param(Enum.valueOf(type, value));
                return;

            } else

            // String
            if (type.equals(String.class)) {

                arr.param(value);
                return;

            } else

            // Class
            if (type.equals(Class.class)) {

                arr.param(this.getClass().getClassLoader().loadClass(value));
                return;

            } else

            // Primitives
            if (type.isPrimitive()) {

                // int
                if (type.getSimpleName().equals("int")) {

                    arr.param(Integer.parseInt(value));
                    return;

                } else

                // boolean
                if (type.getSimpleName().equals("boolean")) {

                    arr.param(Boolean.parseBoolean(value));
                    return;

                }
            }

            throw new ClassNotFoundException("Parameter type " + type.toString() + " is not supported by plugin.");

        }


    }


}
