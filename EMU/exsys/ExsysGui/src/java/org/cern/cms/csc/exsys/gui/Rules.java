/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.context.FacesContext;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import org.cern.cms.csc.exsys.gui.component.BeanTableManager;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */

@ManagedBean
@SessionScoped
public class Rules extends JsfBeanBase {

    @EJB
    private MainBeanTableDaoLocal tableDao;

    private BeanTableManager table;

    public Rules() throws Exception {
        this.table = new BeanTableManager("reRule", Rule.class) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return tableDao;
            }
            @Override
            public FacesContext getContext() {
                return FacesContext.getCurrentInstance();
            }
        };
    }

    public BeanTableManager getTable() {
        return table;
    }

}
