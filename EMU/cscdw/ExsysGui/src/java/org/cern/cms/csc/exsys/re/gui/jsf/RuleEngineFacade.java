/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf;

import java.util.List;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */

@EJB(name="RuleEngineDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal.class)
public class RuleEngineFacade extends JsfBeanBase {

    private static Logger logger = Logger.getLogger(RuleEngineFacade.class.getName());

    RuleEngineDaoLocal reDao;

    /** Creates a new instance of RuleEngineFacade */
    public RuleEngineFacade() throws NamingException {
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
        return reDao.getAllRules().size();
    }

    private RuleEngineDaoLocal getReDao() throws NamingException {
        if (reDao == null) {
            reDao = (RuleEngineDaoLocal) getEjb("RuleEngineDaoRef");
        }
        return reDao;
    }
}
