/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import java.io.Serializable;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.ViewScoped;
import javax.transaction.UserTransaction;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.exsys.gui.util.ClassFinder;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.RuleSet;
import org.icefaces.bean.ViewRetained;
import org.icefaces.bean.WindowDisposed;

/**
 *
 * @author evka
 */
@ManagedBean
@ViewScoped
@ViewRetained
@WindowDisposed
public class RuleSets implements Serializable {

    @Resource
    private UserTransaction ut;
    @EJB
    private RuleEngineDaoLocal reDao;

    @EJB
    private MainBeanTableDaoLocal tableDao;

    private BeanTableManager table;

    /** Creates a new instance of RuleSets */
    public RuleSets() throws Exception{
        this.table = new BeanTableManager("reRuleSets", RuleSet.class) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return tableDao;
            }
            @Override
            public ClassFinderIf getClassFinder() {
                return ClassFinder.getInstance();
            }
        };
    }

    public BeanTableManager getTable() {
        return table;
    }

}
