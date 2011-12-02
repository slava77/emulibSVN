/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import java.beans.PropertyDescriptor;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import javax.persistence.Transient;
import jsf.bean.gui.JsfBeanBase;
import jsf.bean.gui.component.table.export.BeanTableExportColumn;
import org.apache.commons.beanutils.PropertyUtils;

/**
 *
 * @author valdor
 */
@ManagedBean(name="fmComponentBean")
@RequestScoped
public class FmComponentBean extends JsfBeanBase {

    private static final Set<Package> FINAL_PACKAGES;
    static {
        FINAL_PACKAGES = new HashSet<Package>();
        FINAL_PACKAGES.add(Package.getPackage("java.lang"));
        FINAL_PACKAGES.add(Package.getPackage("java.math"));
        FINAL_PACKAGES.add(Package.getPackage("java.math"));
    };

    private static final Set<Class> FINAL_CLASSES;
    static {
        FINAL_CLASSES = new HashSet<Class>();
        FINAL_CLASSES.add(Date.class);
    };

    private static final Set<Class> SKIP_CLASSES;
    static {
        SKIP_CLASSES = new HashSet<Class>();
        SKIP_CLASSES.add(Class.class);
    };

    private static final String DATA_HINT_LABEL = "%s (%s)";
    private static final String TMPL_DATA_HINT_VALUE = "${%s?default('')}";
    private static final String TEST_DATA_HINT_VALUE = "%s";

    /**
     * Retrieves a composite component attribute by name
     * @param name Attribute name
     * @return attribute value
     */
    @SuppressWarnings(value="unchecked")
    private <T> T getAttribute(Class<T> clazz, String name) {
        UIComponent c = UIComponent.getCurrentComponent(FacesContext.getCurrentInstance());
        UIComponent cc = UIComponent.getCompositeComponentParent(c);
        return (T) cc.getAttributes().get(name);
    }

    private UIInput getTemplateInput(UIComponent c) {
        UIComponent cc = UIComponent.getCompositeComponentParent(c);
        return (UIInput) cc.findComponent(cc.getClientId() + ":templateStr");
    }

    /**
     * Is it a test template component?
     * @return
     */
    private boolean isTest() {
        return getAttribute(Boolean.class, "test");
    }


    private void buildRootHelp(String value, Class c, Object v, List<SelectItem> help) {

        String valueFormat = isTest() ? TEST_DATA_HINT_VALUE : TMPL_DATA_HINT_VALUE;

        if (SKIP_CLASSES.contains(c)) {
            return;
        }

        // Final stuff?
        if (FINAL_PACKAGES.contains(c.getPackage())
                || FINAL_CLASSES.contains(c)
                || c.isEnum()
                || BeanTableExportColumn.ColumnValue.class.isAssignableFrom(c)) {

            String typeName = c.getSimpleName().toLowerCase();
            help.add(new SelectItem(String.format(valueFormat, value),
                                    String.format(DATA_HINT_LABEL, value, typeName),
                                    value));

        } else

        // Map
        if (Map.class.isAssignableFrom(c)) {

            Map mv = (Map) v;
            for (Object k: mv.keySet()) {
                buildRootHelp((value == null ? "" : value.concat(".")).concat((String) k),
                              mv.get(k).getClass(),
                              mv.get(k),
                              help);
            }

        } else

        // Array or Collection
        if (c.isArray() || Collection.class.isAssignableFrom(c)) {

            help.add(new SelectItem(String.format(valueFormat, value),
                                    String.format(DATA_HINT_LABEL, value, "sequence"),
                                    value));

            help.add(new SelectItem(String.format(valueFormat, value.concat("?size")),
                                    String.format(DATA_HINT_LABEL, value.concat("?size"), "integer"),
                                    value));

        } else

        // Anything else
        for (PropertyDescriptor pd: PropertyUtils.getPropertyDescriptors(c)) {
            if (!pd.getReadMethod().isAnnotationPresent(Transient.class)) {
                try {
                    String str = (value == null ? "" : value.concat(".")).concat(pd.getName());
                    Class clazz = pd.getPropertyType();
                    Method met = pd.getReadMethod();
                    Object obj = met.invoke(v);
                    if (obj == null
                            && !FINAL_PACKAGES.contains(clazz.getPackage())
                            && !FINAL_CLASSES.contains(clazz)
                            && !Map.class.isAssignableFrom(clazz)
                            && !c.isArray()
                            && !Collection.class.isAssignableFrom(c)
                            && !clazz.isEnum()) {
                        buildRootHelp(str, clazz, clazz.newInstance(), help);
                    } else {
                        buildRootHelp(str, clazz, obj, help);
                    }
                } catch (Exception ex) { }
            }
        }

    }

    public Collection<SelectItem> getRootHelp() {
        List<SelectItem> help = new ArrayList<SelectItem>();
        buildRootHelp(null, Map.class, getAttribute(Map.class, "root"), help);
        Collections.sort(help, new TemplateHelpComparator());
        return help;
    }

    public void compileListener(ActionEvent ev) {
        UIInput input = getTemplateInput(ev.getComponent());
        validateTemplate(FacesContext.getCurrentInstance(), input, input.getSubmittedValue());
        if (input.isValid()) {
            addInfoMessage("Compilation successfull");
        }
    }

    @SuppressWarnings(value="unchecked")
    public void validateTemplate(FacesContext fc, UIComponent uic, Object o) {
        try {
            if (isTest()) {
                TestManager tm = new TestManager();
                tm.addTest("test", (String) o);
                tm.test("test", getAttribute(Map.class, "root"));
            } else {
                TemplateManager tm = new TemplateManager();
                tm.addTemplate("template", (String) o);
                tm.execute("template", getAttribute(Map.class, "root"));
            }
        } catch (Exception ex) {
            ((UIInput) uic).setValid(false);
            JsfBeanBase.addErrorMessage(ex.getMessage());
        }
    }
}
