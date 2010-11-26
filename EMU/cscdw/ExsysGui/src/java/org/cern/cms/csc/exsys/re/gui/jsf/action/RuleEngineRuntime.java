/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import java.util.Collection;
import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
public class RuleEngineRuntime extends JsfBeanBase {

    @EJB
    private RuleEngineManagerLocal reManager;

    /** Creates a new instance of RuleEngineRuntime */
    public RuleEngineRuntime() throws NamingException {
        super();
    }

    public long getNumEventsEvaluated() {
        return reManager.getEsperRuntime().getNumEventsEvaluated();
    }

    public String reconfigure() {
        reManager.reconfigure();
        return "";
    }

    /**
     * Get all rules that are currently active in the RE runtime
     * @return all rules that are currently active in the RE runtime
     */
    public Collection<Rule> getActiveRules() {
        return reManager.getActiveRules();
    }

    /**
     * Get number of active rules in the RE runtime
     * @return number of active rules in the RE runtime
     */
    public int getActiveRulesCount() {
        return reManager.getActiveRules().size();
    }

}
