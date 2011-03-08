/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import java.beans.PropertyDescriptor;
import java.util.ArrayList;
import java.util.Collection;
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

    private static final String DATA_HINT_FORMAT = "%s (%s)";

    private void buildRootHelp(String value, Class c, List<SelectItem> help) {

        if (SKIP_CLASSES.contains(c)) {
            return;
        }

        // Final stuff?
        if (FINAL_PACKAGES.contains(c.getPackage()) ||
            FINAL_CLASSES.contains(c)) {

            String typeName = c.getSimpleName().toLowerCase();
            help.add(new SelectItem(value, String.format(DATA_HINT_FORMAT, value, typeName), value));

        } else {

        // Anything else
            for (PropertyDescriptor pd: PropertyUtils.getPropertyDescriptors(c)) {
                if (!pd.getReadMethod().isAnnotationPresent(Transient.class)) {
                    buildRootHelp(value.concat(".").concat(pd.getName()), pd.getPropertyType(), help);
                }
            }
        }

    }

    private void buildRootHelp(Map root, List<SelectItem> help) {
        for (Object k: root.keySet()) {
            buildRootHelp((String) k, root.get(k).getClass(), help);
        }
    }

    public Collection<SelectItem> getRootHelp() {
        List<SelectItem> help = new ArrayList<SelectItem>();
        Map root = getRoot(UIComponent.getCurrentComponent(FacesContext.getCurrentInstance()));
        buildRootHelp(root, help);
        return help;
    }

    private Map getRoot(UIComponent byComponent) {
        UIComponent cc = UIComponent.getCompositeComponentParent(byComponent);
        return (Map) cc.getAttributes().get("root");
    }

    private UIInput getTestStrInput(UIComponent byComponent) {
        String compositeId = UIComponent.getCompositeComponentParent(byComponent).getClientId();
        return (UIInput) FacesContext.getCurrentInstance().getViewRoot().findComponent(compositeId + ":testStr");
    }

    public void compileListener(ActionEvent ev) {
        UIInput input = getTestStrInput(ev.getComponent());
        validateTest(FacesContext.getCurrentInstance(), input, input.getSubmittedValue());
        if (input.isValid()) {
            addInfoMessage("Compilation successfull");
        }
    }

    public void validateTest(FacesContext fc, UIComponent uic, Object o) {
        UIInput uicIn = (UIInput) uic;
        TestManager tm = new TestManager();
        try {
            tm.addTest("test", (String) o);
            tm.test("test", getRoot(uic));
        } catch (Exception ex) {
            uicIn.setValid(false);
            JsfBeanBase.addErrorMessage(ex.getMessage());
        }
    }
}
