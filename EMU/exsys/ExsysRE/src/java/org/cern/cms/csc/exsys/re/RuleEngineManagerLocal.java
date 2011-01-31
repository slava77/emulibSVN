/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.EPRuntime;
import java.util.Collection;
import javax.ejb.Local;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
@Local
public interface RuleEngineManagerLocal {

    /**
     * @return Esper Runtime (EPRuntime) which can be used to send facts into rule engine.
     */
    EPRuntime getEsperRuntime();

    /**
     * Calls reconfigure() for the default EPServiceProvider.
     * @see reconfigure(String uri)
     */
    void reconfigure();

    /**
     * Removes all rules from the EPServiceProvider with the given uri and calls configure (which adds all the rules from DB).
     * @see configure(EPServiceProvider epService)
     */
    void reconfigure(String uri);

//    /** Configures the given EPServiceProvider i.e. gets all the rules from DB and registers them with the given EPServiceProvider. */
//    void configure(EPServiceProvider epService);

    /**
     * Get all rules that are currently active in the RE runtime
     * @return all rules that are currently active in the RE runtime
     */
    Collection<Rule> getActiveRules();

    RuleEngineDaoLocal getRuleEngineDao();
    
}
