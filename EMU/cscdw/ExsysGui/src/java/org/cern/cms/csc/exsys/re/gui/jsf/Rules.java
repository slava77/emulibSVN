/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf;

import java.util.Collections;
import java.util.List;
import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */

@EJB(name="RuleEngineDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal.class)
public class Rules extends JsfBeanBase {

    private ConclusionType conclusionType;
    
    private RuleEngineDaoLocal reDao;

    /** Creates a new instance of RuleEngineFacade */
    public Rules() {
        super();
    }

    public List<Rule> getAllRules() throws NamingException {
        return getReDao().getAllRules();
    }

    public List<Rule> getAllRulesForConclType() throws NamingException {
        if (conclusionType == null) {
            return Collections.EMPTY_LIST;
        }
        return getReDao().getRulesByConclusionType(conclusionType);
    }

    public ConclusionType getConclusionType() {
        return conclusionType;
    }

    public void setConclusionType(ConclusionType conclusionType) {
        this.conclusionType = conclusionType;
    }

    private RuleEngineDaoLocal getReDao() throws NamingException {
        if (reDao == null) {
            reDao = (RuleEngineDaoLocal) getEjb("RuleEngineDaoRef");
        }
        return reDao;
    }
}
