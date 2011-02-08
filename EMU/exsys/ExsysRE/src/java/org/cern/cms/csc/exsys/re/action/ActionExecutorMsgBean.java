/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action;

import javax.ejb.ActivationConfigProperty;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.exsys.re.model.ActionExecution;

/**
 *
 * @author evka
 */
@MessageDriven(mappedName = "jms/ruleEngineActionQueue", activationConfig =  {
        @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge"),
        @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue")
    })
public class ActionExecutorMsgBean implements MessageListener {

    private static final Logger logger = SimpleLogger.getLogger(ActionExecutorMsgBean.class);

    public ActionExecutorMsgBean() {
    }

    public void onMessage(Message message) {
        try {
            ObjectMessage omessage = (ObjectMessage) message;
            if (omessage.getObject() instanceof ActionExecution) {
                ActionExecution exec = (ActionExecution) omessage.getObject();
            } else {
                logger.error("Rule Engine Input Message Bean got an unrecognized object (type: " +
                                omessage.getObject().getClass().getName() + ") - ignoring");
            }
        } catch (Exception ex) {
            logger.error("Exception in FactCollectionMessageBean", ex);
        }
    }

    /**
     * Executes the given action.
     * @param actionExec action to execute.
     */
    public void executeAction(ActionExecution actionExec) {
        logger.info("Got action exec with action=" + actionExec.getAction().getName() + " and trigger id = " + actionExec.getTrigger().getid());
//        if (!action.getTriggerType().equals(trigger.getType())) {
//            return;
//        }
//
//        if (action instanceof EmailAction) {
//            EmailAction emailAct = (EmailAction) action;
//            logger.info("ActionExecutor: Executing action '" + action.getName() + "' (" +
//                    trigger.getType().getValue() + "): sending email to " + emailAct.getRecipients());
//            String email, subject, body;
//            email = "evka@cern.ch";
//            subject = "test";
//            body = "test";
//            try {
//                sendMail(email, subject, body);
//            } catch (Exception ex) {
//                logger.error("Exception while sending email to " + email + ", with subject '" + subject + "' and body '" + body + "'", ex);
//            }
//        } else if (action instanceof SmsAction) {
//            SmsAction smsAct = (SmsAction) action;
//            logger.info("ActionExecutor: Executing action '" + action.getName() + "' (" +
//                    trigger.getType().getValue() + "): sending SMS to " + smsAct.getRecipients());
//        } else {
//            throw new RuntimeException("Unrecognized action type (" + action.getClass().getSimpleName() + ")- cannot execute");
//        }
//
//        ActionExecution exec = new ActionExecution();
//        exec.setAction(action);
//        exec.setTimestampItem(new Date());
//        exec.setTrigger(trigger);
//        try {
//            entityDao.getPersistDao().persist(exec);
//        } catch (Exception ex) {
//            logger.error("Exception while persisting action execution object (action was executed though)", ex);
//        }
    }
    
}
