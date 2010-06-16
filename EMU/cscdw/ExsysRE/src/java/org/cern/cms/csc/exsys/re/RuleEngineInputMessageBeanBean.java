/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
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

    private static Logger logger = Logger.getLogger(RuleEngineInputMessageBeanBean.class.getName());

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
                logger.warning("Rule Engine Input Message Bean got an unrecognized object (type: " +
                                omessage.getObject().getClass().getName() + ") - ignoring");
            }
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception in FactCollectionMessageBean", ex);
        }
    }
    
}
