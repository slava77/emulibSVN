/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component;

import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import jsf.bean.gui.converter.NewLineConverter;

/**
 *
 * @author valdo
 */
@ManagedBean
@RequestScoped
public class ComponentSupportBean {

    public String getClientId() {
        UIComponent c = UIComponent.getCurrentComponent(FacesContext.getCurrentInstance());
        return UIComponent.getCompositeComponentParent(c).getClientId();
    }

    public String getJqPrefix() {
        return "#".concat(getClientId()).concat(":").replaceAll(":", "\\\\\\\\:");
    }

    public NewLineConverter getNewLineConverter() {
        return new NewLineConverter();
    }

}
