package org.cern.cms.csc.exsys.gui.util;

import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.convert.Converter;

public class NewLineConverter implements Converter {

    @Override
    public Object getAsObject(FacesContext context, UIComponent component, String value) {
        String s = (String) value;
        return s.replaceAll("\\<br/>","\n");
    }

    @Override
    public String getAsString(FacesContext context, UIComponent component, Object value) {
        String s = (String) value;
        return s.replaceAll("\n", "<br/>");
    }

}
