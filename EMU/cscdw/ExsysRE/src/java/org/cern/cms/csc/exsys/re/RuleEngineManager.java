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
import java.util.List;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import org.cern.cms.csc.exsys.re.conclusion.factory.ConclusionFactory;
import org.cern.cms.csc.exsys.re.conclusion.factory.DefaultConclusionFactory;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author evka
 */
@Stateless
public class RuleEngineManager implements RuleEngineManagerLocal, RuleEngineManagerRemote {

    private static Logger logger = Logger.getLogger(RuleEngineManager.class.getName());

    private final String DEFAULT_EP_URI = "exsys";
    private final String FACTS_PACKAGE = "org.cern.cms.csc.dw.model.fact";
    private final String CONCLUSION_PACKAGE = "org.cern.cms.csc.exsys.re.model";
    private EPServiceProvider epService;

    @EJB
    private RuleEngineDaoLocal reDao;

    public RuleEngineManager() {}

    private EPServiceProvider getEpService() {
        return getEpService(DEFAULT_EP_URI);
    }

    private EPServiceProvider getEpService(String uri) {
        if (epService == null) {
            String[] uris = EPServiceProviderManager.getProviderURIs();
            boolean serviceExists = false;
            for (String currentUri: uris) {
                if (currentUri.equals(uri)) {
                    serviceExists = true;
                    break;
                }
            }

            if (serviceExists) { // Esper service already exists so just and return it
                epService = EPServiceProviderManager.getProvider(uri);
            } else { // create and configure the Esper service
                logger.info("Creating and configuring Esper service");
                Configuration conf = new Configuration();
                conf.addEventTypeAutoName(FACTS_PACKAGE);
                conf.addEventTypeAutoName(CONCLUSION_PACKAGE);
                epService = EPServiceProviderManager.getProvider(uri, conf);
                configureEsper(epService);
                logger.info("Esper service initialized and configured");
            }

            //TODO revisit rule setup at startup
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
        return epService;
    }

    private void configureEsper(EPServiceProvider epService) {
        List<Rule> rules = reDao.getAllRules();
        logger.info("These are the rules to be loaded: ");
        for (Rule rule: rules) {
            logger.info("Rule: " + rule.getName());
            ConclusionFactory factory = new DefaultConclusionFactory(rule.getConclusionType());
            EPStatement statement = epService.getEPAdministrator().createEPL(rule.getRuleDefinition(), rule.getName());
            statement.addListener(factory);
        }
    }

    public void reconfigureEsper() {
        if (epService != null) {
            epService.getEPAdministrator().destroyAllStatements();
            configureEsper(epService);
        }
    }

    /**
     * @return Esper Runtime (EPRuntime) which can be used to send facts into rule engine.
     */
    public EPRuntime getEsperRuntime() {
        return getEpService().getEPRuntime();
    }
}
