/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.bean.ViewScoped;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import org.cern.cms.csc.dw.util.ClassFinder;
import org.cern.cms.csc.exsys.re.model.Action;
import org.icefaces.bean.ViewRetained;
import org.icefaces.bean.WindowDisposed;

/**
 *
 * @author Evka
 */

@ManagedBean
@ViewScoped
@ViewRetained
@WindowDisposed
public class Actions extends JsfBeanBase {

    @EJB
    private MainBeanTableDaoLocal dao;

    private BeanTableManager table;

    public Actions() throws Exception {
        this.table = new BeanTableManager("reActions", Action.class) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return dao;
            }
            @Override
            public ClassFinderIf getClassFinder() {
                return ClassFinder.getInstance();
            }
        };
    }

    public BeanTableManager getTable() throws Exception {
        return table;
    }

}
