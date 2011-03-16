/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author Evka
 */
@Stateless
@EJB(name="ejb/OntologyDao", beanInterface=org.cern.cms.csc.dw.dao.OntologyDaoLocal.class)
public class RuleEngineInput implements RuleEngineInputRemote, RuleEngineInputLocal {

    private static Logger logger = SimpleLogger.getLogger(RuleEngineInput.class);

    @EJB
    private RuleEngineManagerLocal reManager;

    @Override
    public void receiveFact(Fact fact) {
        reManager.postEvent(fact);
        logger.trace("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to Esper");
    }

    @Override
    public void receiveConclusion(Conclusion conclusion) {
        throw new UnsupportedOperationException("Receiving conclusions in the rule engine is not yet implemented");
        //getEpService().getEPRuntime().sendEvent(conclusion);
        //System.out.println("ExsysRE: received a fact of type " + fact.getClass().getName() + " and sent to CEP");
    }
}
