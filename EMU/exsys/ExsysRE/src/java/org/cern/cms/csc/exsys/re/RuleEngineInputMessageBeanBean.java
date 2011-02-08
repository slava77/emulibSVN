/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.fact.Fact;

/**
 *
 * @author Evka
 */
@MessageDriven(mappedName = "jms/ruleEngineInputQueue", activationConfig =  {
        @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge"),
        @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue")
    })
public class RuleEngineInputMessageBeanBean implements MessageListener {

    private static Logger logger = SimpleLogger.getLogger(RuleEngineInputMessageBeanBean.class);

    @EJB
    RuleEngineInputLocal reInput;

    public RuleEngineInputMessageBeanBean() {
    }

    public void onMessage(Message message) {
        try {
            ObjectMessage omessage = (ObjectMessage) message;
            if (omessage.getObject() instanceof Fact) {
                Fact fact = (Fact) omessage.getObject();
                reInput.receiveFact(fact);
            } else {
                logger.error("Rule Engine Input Message Bean got an unrecognized object (type: " +
                                omessage.getObject().getClass().getName() + ") - ignoring");
            }
        } catch (Exception ex) {
            logger.error("Exception in FactCollectionMessageBean", ex);
        }
    }
    
}
