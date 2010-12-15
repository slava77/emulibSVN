/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.ejb.EJB;
import javax.faces.convert.Converter;
import javax.faces.model.SelectItem;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.convert.LovConverter;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author Evka
 */

@EJB(name="ConclusionDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal.class)
public class Conclusions extends JsfBeanBase {

    private ConclusionDaoLocal conclDao;

    private DisplayMode displayMode;

    /** Creates a new instance of RuleEngineFacade */
    public Conclusions() {
        super();
        displayMode = DisplayMode.CURRENT;
    }

    public List<Conclusion> getConclusions() throws NamingException {
        if (displayMode == DisplayMode.CURRENT) {
            return getConclusionDao().getOpenTopConclusions(false);
        } else if (displayMode == DisplayMode.ACKNOWLEDGED) {
            return getConclusionDao().getOpenTopConclusions(true);
        } else if (displayMode == DisplayMode.CLOSED) {
            return getConclusionDao().getAllClosedTopConclusions();
        }
        return Collections.EMPTY_LIST;
    }

    private ConclusionDaoLocal getConclusionDao() throws NamingException {

        if (conclDao == null) {
            conclDao = (ConclusionDaoLocal) getEjb("ConclusionDaoRef");
        }
        return conclDao;
    }

    public DisplayMode getDisplayMode() {
        return displayMode;
    }

    public void setDisplayMode(DisplayMode displayMode) {
        this.displayMode = displayMode;
    }

    public List<SelectItem> getAllDisplayModes() {
        List<SelectItem> ret = new ArrayList<SelectItem>();
        for (DisplayMode dm: DisplayMode.values()) {
            ret.add(new SelectItem(dm, dm.getValue()));
        }
        return ret;
    }

    public enum DisplayMode {
        CURRENT("Current"),
        ACKNOWLEDGED("Acknowledged"),
        CLOSED("Closed");
        private final String value;

        DisplayMode(String v) {
            value = v;
        }

        public String value() {
            return value;
        }

        public String getValue() {
            return value();
        }

        public static DisplayMode fromValue(String v) {
            for (DisplayMode c: DisplayMode.values()) {
                if (c.value.equals(v)) {
                    return c;
                }
            }
            throw new IllegalArgumentException(v);
        }

    }
}
