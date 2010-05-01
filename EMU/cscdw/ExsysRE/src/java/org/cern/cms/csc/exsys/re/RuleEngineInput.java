/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

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

    @EJB
    private RuleEngineManagerLocal reManager;

    public void receiveFact(Fact fact) {
        reManager.getEsperRuntime().sendEvent(fact);
        //System.out.println("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to CEP");
    }

    public void receiveConclusion(Conclusion conclusion) {
        //getEpService().getEPRuntime().sendEvent(conclusion);
        //System.out.println("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to CEP");
    }
}
