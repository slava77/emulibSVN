/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author Evka
 */
@Stateless
public class RuleEngineInput implements RuleEngineInputRemote, RuleEngineInputLocal {

    private static Logger logger = Logger.getLogger(RuleEngineInput.class.getName());

    @EJB
    private RuleEngineManagerLocal reManager;

    public void receiveFact(Fact fact) {
        reManager.getEsperRuntime().sendEvent(fact);
        logger.fine("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to Esper");
    }

    public void receiveConclusion(Conclusion conclusion) {
        throw new UnsupportedOperationException("Receiving conclusions in the rule engine is not yet implemented");
        //getEpService().getEPRuntime().sendEvent(conclusion);
        //System.out.println("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to CEP");
    }
}
