/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import javax.ejb.EJB;
import javax.enterprise.context.RequestScoped;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.bean.ViewScoped;
import javax.faces.model.SelectItem;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.BeanTableFilter.Operation;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import org.cern.cms.csc.exsys.gui.util.ClassFinder;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author Evka
 */

@ManagedBean
@ViewScoped
public class Conclusions extends JsfBeanBase implements Serializable {

    @EJB
    private MainBeanTableDaoLocal dao;
    private BeanTableManager table;

    private DisplayMode displayMode;

    /** Creates a new instance of RuleEngineFacade */
    public Conclusions() throws Exception {
        super();
        this.table = new BeanTableManager("reConclusions", Conclusion.class) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return dao;
            }
            @Override
            public ClassFinderIf getClassFinder() {
                return ClassFinder.getInstance();
            }
        };
        setDisplayMode(DisplayMode.CURRENT, false);
    }

    public BeanTableManager getTable() {
        return table;
    }

    public DisplayMode getDisplayMode() {
        return displayMode;
    }

    public void setDisplayMode(DisplayMode displayMode) {
        setDisplayMode(displayMode, true);
    }

    public void setDisplayMode(DisplayMode displayMode, boolean refreshData) {
        this.displayMode = displayMode;
        table.removeAllPropertyFilters();
        if (displayMode.equals(DisplayMode.CURRENT)) {
            table.addPropertyFilter("closed", Operation.EQUAL, false);
            table.addPropertyFilter("acknowledged", Operation.EQUAL, false);
        } else if (displayMode.equals(DisplayMode.ACKNOWLEDGED)) {
            table.addPropertyFilter("closed", Operation.EQUAL, false);
            table.addPropertyFilter("acknowledged", Operation.EQUAL, true);
        } else if (displayMode.equals(DisplayMode.CLOSED)) {
            table.addPropertyFilter("closed", Operation.EQUAL, true);
        }
        if (refreshData) {
            table.getTable().refresh();
        }
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
