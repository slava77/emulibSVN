/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.Configuration;
import com.espertech.esper.client.EPRuntime;
import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPServiceProviderManager;
import com.espertech.esper.client.EPStatement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.conclusion.factory.ConclusionFactory;
import org.cern.cms.csc.exsys.re.conclusion.factory.DefaultConclusionFactory;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author evka
 * This is a bean dealing with Esper runtime (e.g. configuring it with rules.
 */
@Stateless
public class RuleEngineManager implements RuleEngineManagerLocal {

    private static Logger logger = Logger.getLogger(RuleEngineManager.class.getName());

    public static final String DEFAULT_EP_URI = "exsys";
    public static final String FACTS_PACKAGE = "org.cern.cms.csc.dw.model.fact";
    public static final String CONCLUSION_PACKAGE = "org.cern.cms.csc.exsys.re.model";

    @Resource(name="EsperServiceProvider")
    private EsperServiceProvider epProvider;

    private EPServiceProvider defaultEpServiceCache;
    
    @EJB
    private RuleEngineDaoLocal reDao;

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
        List<Rule> rules = reDao.getAllRules();
        logger.info("Configuring rules engine... Loading these rules: ");
        for (Rule rule: rules) {
            logger.info("Rule: " + rule.getName());
            ConclusionFactory factory = new DefaultConclusionFactory(rule.getConclusionType());
            EPStatement statement = epService.getEPAdministrator().createEPL(rule.getRuleDefinition(), rule.getName());
            statement.addListener(factory);
        }
    }


    /**
     * Calls reconfigure() for the default EPServiceProvider.
     * @see reconfigure(String uri)
     */
    public void reconfigure() {
        reconfigure(DEFAULT_EP_URI);
    }

    /**
     * Removes all rules from the EPServiceProvider with the given uri and calls configure (which adds all the rules from DB).
     * @see configure(EPServiceProvider epService)
     */
    public void reconfigure(String uri) {
        logger.info("Reconfiguring Esper Runtime for URI=" + uri);
        EPServiceProvider epService = getEpService(uri);
        epService.getEPAdministrator().destroyAllStatements();
        configure(epService);
    }

    /**
     * @return Esper Runtime (EPRuntime) which can be used to send facts into rule engine.
     */
    public EPRuntime getEsperRuntime() {
        return getEpService().getEPRuntime();
    }
}
