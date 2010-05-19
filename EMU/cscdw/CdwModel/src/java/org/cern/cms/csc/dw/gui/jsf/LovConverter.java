/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.gui.jsf;

import java.util.List;
import java.util.logging.Logger;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.convert.Converter;

/**
 * Converter used in list of value select boxes and menus
 * @author Evka
 */
public class LovConverter implements Converter {

    private static Logger logger = Logger.getLogger(LovConverter.class.getName());

    private List<Object> lov = null;

    public LovConverter(List<Object> lov) {
        this.lov = lov;
    }

    public Object getAsObject(FacesContext facesContext, UIComponent uiComponent, String param) {
        try {
            Integer index = Integer.valueOf(param);
            return lov.get(index);
        } catch (NumberFormatException ex) {
            throw new RuntimeException("Number format exception in LovConverter.getAsObject() while parsing " + param, ex);
        }
    }

    public String getAsString(FacesContext facesContext, UIComponent uiComponent, Object obj) {
        Integer index = new Integer(lov.indexOf(obj));
        return index.toString();
    }
}
