/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import java.util.Collection;
import javax.ejb.Local;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;

/**
 *
 * @author Evka
 */
@Local
public interface RuleEngineManagerLocal {

    /**
     * Posts the given event to the rule engine.
     * This method should be used when posting events from within rule engine listeners (like ConclusionFactory).
     * @param event event that you want to post
     */
    void postEventFromListener(Object event);

    /**
     * Posts the given event to the rule engine
     * @param event event that you want to post
     */
    void postEvent(Object event);

    /**
     * Calls reconfigure() for the default EPServiceProvider.
     * @see reconfigure(String uri)
     */
    void reconfigure();

    /**
     * Reconfigures RE for fact replay.
     * This means that internal timer will be disabled and time will be taken from the incoming facts.
     */
    void reconfigureForFactReplay();

    /**
     * Removes all rules from the EPServiceProvider with the given uri and calls configure (which adds all the rules from DB).
     * @see configure(EPServiceProvider epService)
     */
    void reconfigure(String uri);

//    /** Configures the given EPServiceProvider i.e. gets all the rules from DB and registers them with the given EPServiceProvider. */
//    void configure(EPServiceProvider epService);

    RuleEngineDaoLocal getRuleEngineDao();
    
    /**
     * Get all rules that are currently active in the RE runtime
     * @return all rules that are currently active in the RE runtime
     */
    Collection<Rule> getActiveRules();

    Collection<RuleSet> getActiveRuleSets();

    long getNumEventsEvaluated();
}
