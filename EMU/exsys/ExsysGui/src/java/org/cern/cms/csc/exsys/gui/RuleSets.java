/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import java.io.Serializable;
import java.util.Collections;
import java.util.List;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.bean.ViewScoped;
import javax.faces.convert.Converter;
import javax.transaction.UserTransaction;
import javax.xml.bind.JAXBException;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.exsys.gui.util.ClassFinder;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDao;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoRemote;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;
import org.icefaces.bean.ViewRetained;
import org.icefaces.bean.WindowDisposed;
import org.xml.sax.SAXException;

/**
 *
 * @author evka
 */
@ManagedBean
@ViewScoped
@ViewRetained
@WindowDisposed
public class RuleSets implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(RuleSets.class);

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

    public void exportToXml() throws SAXException, JAXBException {
        RuleSet ruleSet = (RuleSet) table.getSelected();
        if (ruleSet != null) {
            try {
                ut.begin();
                ruleSet = (RuleSet) reDao.getEntityDao().refreshEntity(ruleSet);
                for (ConclusionType ct: ruleSet.getConclusionTypes()) { // load lazy relations
                    for (Rule r: ct.getRules()) {
                        r.getConclusionType();
                    }
                }
                ut.commit();

                EjbLookup<RuleEngineDaoRemote> reDaoRemote = new EjbLookup<RuleEngineDaoRemote>(RuleEngineDaoRemote.class,
                                                                                                RuleEngineDao.class,
                                                                                                "Exsys",
                                                                                                "ExsysModel",
                                                                                                "emuslice21",
                                                                                                3700);
                logger.info("va:");
                for (RuleSet rs: reDaoRemote.ejb().getAllRuleSets()) {
                    logger.info(rs);
                }
logger.info("URA!!! :)");
            } catch (Exception ex) {
                logger.error(ex);
            }
        }
    }
}
