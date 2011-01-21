/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.converter;

import java.util.Arrays;
import java.util.List;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.convert.Converter;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;

/**
 * Converter used in list of value select boxes and menus
 * @author Evka
 */
public class LovConverter implements Converter {

    private static Logger logger = SimpleLogger.getLogger(LovConverter.class);

    private List lov = null;

    public LovConverter(Object[] lov) {
        this.lov = Arrays.asList(lov);
    }

    public LovConverter(List lov) {
        this.lov = lov;
//        logger.info("Creating new Lov converter with these values: " + lov);
    }

    @Override
    public Object getAsObject(FacesContext facesContext, UIComponent uiComponent, String param) {
        try {
            Integer index = Integer.valueOf(param);
//            logger.info("LovConverter.getAsObject(). Param=" + param + ", index: " + index + " returning: " + lov.get(index));
            return lov.get(index);
        } catch (NumberFormatException ex) {
            throw new RuntimeException("Number format exception in LovConverter.getAsObject() while parsing " + param, ex);
        }
    }

    @Override
    public String getAsString(FacesContext facesContext, UIComponent uiComponent, Object obj) {
        Integer index = new Integer(lov.indexOf(obj));
//        logger.info("LovConverter.getAsString(). obj=" + obj + ", index: " + index + " returning: " + index.toString());
        if (index < 0) { index = 0; } // back to first element if the obj was actually not found (needed because icefaces still sometimes sends some old version of the object taken from god knows where).
        return index.toString();
    }

}
