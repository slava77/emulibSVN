/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;

/**
 *
 * @author Evka
 */

@EJB(name="ejb/RuleEngineManager", beanInterface=org.cern.cms.csc.exsys.re.RuleEngineManagerLocal.class)
public class RuleEngineRuntime extends JsfBeanBase {

    @EJB
    RuleEngineManagerLocal reManager;

    /** Creates a new instance of RuleEngineRuntime */
    public RuleEngineRuntime() throws NamingException {
        super();
        reManager = (RuleEngineManagerLocal) getEjb("ejb/RuleEngineManager");
    }

    public long getNumEventsEvaluated() {
        return reManager.getEsperRuntime().getNumEventsEvaluated();
    }

    public String reconfigure() {
        reManager.reconfigureEsper();
        return "";
    }
}
