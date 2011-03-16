/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.EPRuntime;
import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPStatement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheServiceLocal;
import org.cern.cms.csc.exsys.re.conclusion.factory.ConclusionFactory;
import org.cern.cms.csc.exsys.re.conclusion.factory.DefaultConclusionFactory;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;

/**
 *
 * @author evka
 * This is a bean dealing with Esper runtime (e.g. configuring it with rules.
 */
@Stateless
public class RuleEngineManager implements RuleEngineManagerLocal {

    private static Logger logger = SimpleLogger.getLogger(RuleEngineManager.class);

    public static final String DEFAULT_EP_URI = "exsys";

    @EJB
    private EsperServiceProvider epProvider;

    private EPServiceProvider defaultEpServiceCache;
    
    @EJB
    private RuleEngineDaoLocal reDao;

    @EJB
    private ConclusionCacheServiceLocal conclusionCacheService;

    private Collection<Rule> activeRules = new ArrayList<Rule>();
    private Collection<RuleSet> activeRuleSets = new ArrayList<RuleSet>();

    public RuleEngineManager() {
    }

    private EPServiceProvider getEpService() {
        if (defaultEpServiceCache == null) {
            defaultEpServiceCache = getEpService(DEFAULT_EP_URI);
        }
        return defaultEpServiceCache;
    }

    private EPServiceProvider getEpService(String uri) {
        boolean needsConfiguration = !epProvider.epServiceExists(uri);
        EPServiceProvider epService = epProvider.getEpService(uri);
        if (needsConfiguration) {
            configure(epService);
        }
        return epService;


// example configuration
//            String rule = "select component from SlidingTmbTriggerCounterFactType where alctCount < 1";
//            String ruleName ="testRule";
//            String title = "Alct trigger rate is very low for $component";
//            String desc = "";
//            ConclusionMaker conclusionMaker = new DefaultConclusionMaker("testConclusionType", title, desc);
//            EPStatement statement = epService.getEPAdministrator().createEPL(rule, ruleName);
//            statement.addListener(conclusionMaker);
//
//            System.out.println("ExsysRE: Rules registered: " + epService.getEPAdministrator().getStatementNames());
    }

    /** Configures the given EPServiceProvider i.e. gets all the rules from DB and registers them with the given EPServiceProvider. */
    private void configure(EPServiceProvider epService) {
        List<RuleSet> ruleSets = reDao.getAllEnabledRuleSets();
        logger.info("Configuring rules engine... Loading these rules: ");
        Set<String> ruleSetNames = new HashSet<String>();
        activeRuleSets.clear();
        activeRules.clear();
        conclusionCacheService.clear();
        for (RuleSet ruleSet: ruleSets) {
            logger.info("  Rule Set: " + ruleSet.toString());
            if (ruleSetNames.contains(ruleSet.getName())) {
                throw new RuntimeException("More than one version of rule set \"" + ruleSet.getName() + "\" is enabled");
            }
            ruleSetNames.add(ruleSet.getName());

            List<Rule> rules = reDao.getEnabledRules(ruleSet);
            Set<String> ruleNames = new HashSet<String>();

            for (Rule rule: rules) {
                if (!rule.isEnabled()) {
                    continue;
                }
                if (ruleNames.contains(rule.getName())) {
                    throw new RuntimeException("There's more than one rule with the same name in the same rule set (" + ruleSet + "): \"" + rule.getName() + "\"");
                }
                ruleNames.add(rule.getName());
                logger.info("    Rule activated: " + rule.getName());
                ConclusionFactory factory = new DefaultConclusionFactory(this, rule, conclusionCacheService);
                EPStatement statement = epService.getEPAdministrator().createEPL(rule.getRuleDefinition(), ruleSet.toString() + "_" + rule.getName());
                statement.addListener(factory);
                activeRules.add(rule);
            }
        }

        // throw all open conclusions into rule engine
        List<Conclusion> openConclusions = reDao.getAllOpenConclusions();
        for (Conclusion concl: openConclusions) {
            epService.getEPRuntime().sendEvent(concl);
        }
    }


    /**
     * Calls reconfigure() for the default EPServiceProvider.
     * @see reconfigure(String uri)
     */
    @Override
    public void reconfigure() {
        reconfigure(DEFAULT_EP_URI);
    }

    /**
     * Removes all rules from the EPServiceProvider with the given uri and calls configure (which adds all the rules from DB).
     * @see configure(EPServiceProvider epService)
     */
    @Override
    public void reconfigure(String uri) {
        logger.info("Reconfiguring Esper Runtime for URI=" + uri);
        EPServiceProvider epService = getEpService(uri);
        epService.getEPAdministrator().destroyAllStatements();
        configure(epService);
    }

    /**
     * @return Esper Runtime (EPRuntime) which can be used to send facts into rule engine.
     */
    private EPRuntime getEsperRuntime() {
        return getEpService().getEPRuntime();
    }

    @Override
    public void postEvent(Object event) {
        getEsperRuntime().sendEvent(event);
    }

    @Override
    public void postEventFromListener(Object event) {
        getEsperRuntime().route(event);
    }

    /**
     * Get all rules that are currently active in the RE runtime
     * @return all rules that are currently active in the RE runtime
     */
    @Override
    public Collection<Rule> getActiveRules() {
        return activeRules;
    }

    @Override
    public Collection<RuleSet> getActiveRuleSets() {
        return activeRuleSets;
    }

    @Override
    public RuleEngineDaoLocal getRuleEngineDao() {
        return reDao;
    }

    @Override
    public long getNumEventsEvaluated() {
        return getEsperRuntime().getNumEventsEvaluated();
    }

}
