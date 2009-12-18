/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.Configuration;
import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPServiceProviderManager;
import com.espertech.esper.client.EPStatement;
import javax.ejb.Stateless;
import org.cern.cms.csc.dw.model.FactType;
import org.cern.cms.csc.exsys.re.conclusion.maker.ConclusionMaker;
import org.cern.cms.csc.exsys.re.conclusion.maker.DefaultConclusionMaker;

/**
 *
 * @author evka
 */
@Stateless
public class RuleEngineBean implements RuleEngineLocal, RuleEngineRemote {

    private final String FACTS_PACKAGE = "org.cern.cms.csc.dw.model";
    private EPServiceProvider epService;

    public RuleEngineBean() {}

    public void sendFact(FactType fact) {
        getEpService().getEPRuntime().sendEvent(fact);
        System.out.println("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to CEP");
    }

    public void addRule(String rule, String ruleName, ConclusionMaker conclusionMaker) {
        EPStatement statement = getEpService().getEPAdministrator().createEPL(rule, ruleName);
        statement.addListener(conclusionMaker);
    }


    private EPServiceProvider getEpService() {
        if (epService == null) {
            Configuration conf = new Configuration();
            conf.addEventTypeAutoName(FACTS_PACKAGE);
            epService = EPServiceProviderManager.getProvider("exsys", conf);

            System.out.println("Registering a testing rule");

            //TODO revisit rule setup at startup
            String rule = "select component from SlidingTmbTriggerCounterFactType where alctCount < 1";
            String ruleName ="testRule";
            String title = "Alct trigger rate is very low for $component";
            String desc = "";
            ConclusionMaker conclusionMaker = new DefaultConclusionMaker("testConclusionType", title, desc);
            EPStatement statement = epService.getEPAdministrator().createEPL(rule, ruleName);
            statement.addListener(conclusionMaker);

            System.out.println("ExsysRE: Rules registered: " + epService.getEPAdministrator().getStatementNames());
        }
        return epService;
    }
}
