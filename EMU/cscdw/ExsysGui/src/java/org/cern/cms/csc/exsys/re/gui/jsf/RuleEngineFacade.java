/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf;

import java.util.List;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.EJBs;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */

@EJBs({
    @EJB(name="RuleEngineDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal.class),
    @EJB(name="ConclusionDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal.class)
})
public class RuleEngineFacade extends JsfBeanBase {

    private static Logger logger = Logger.getLogger(RuleEngineFacade.class.getName());

    RuleEngineDaoLocal reDao;
    ConclusionDaoLocal conclDao;

    /** Creates a new instance of RuleEngineFacade */
    public RuleEngineFacade() {
        super();
    }

    public List<Rule> getAllRules() throws NamingException {
        return getReDao().getAllRules();
    }

    public int getRuleCount() throws NamingException {
        List<Rule> rules = getReDao().getAllRules();
        if (rules == null) {
            return 0;
        }
        return rules.size();
    }

    public List<Conclusion> getAllOpenTopConclusions() throws NamingException {
        return getConclusionDao().getAllOpenTopConclusions();
    }

    private RuleEngineDaoLocal getReDao() throws NamingException {
        if (reDao == null) {
            reDao = (RuleEngineDaoLocal) getEjb("RuleEngineDaoRef");
        }
        return reDao;
    }

    private ConclusionDaoLocal getConclusionDao() throws NamingException {
        if (conclDao == null) {
            conclDao = (ConclusionDaoLocal) getEjb("ConclusionDaoRef");
        }
        return conclDao;
    }

}
