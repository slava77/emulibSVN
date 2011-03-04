/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action;

import java.util.Date;
import javax.annotation.Resource;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.ejb.Timeout;
import javax.ejb.Timer;
import javax.ejb.TimerConfig;
import javax.ejb.TimerService;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheServiceLocal;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.ActionExecution;
import org.cern.cms.csc.exsys.re.model.CommandAction;
import org.cern.cms.csc.exsys.re.model.Conclusion;

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

    @Resource
    private TimerService timerService;
    @EJB
    private ConclusionCacheServiceLocal conclCache;
    @EJB
    private RuleEngineDaoLocal reDao;
    @EJB
    private RuleEngineManagerLocal reManager;

    public ActionExecutorMsgBean() {
    }

    public void onMessage(Message message) {
        try {
            ObjectMessage omessage = (ObjectMessage) message;
            if (omessage.getObject() instanceof ActionExecution) {
                ActionExecution exec = (ActionExecution) omessage.getObject();
                long timeout = exec.getAction().getTimeout();
                if (timeout > 0) {
                    Timer timer = timerService.createSingleActionTimer(timeout, new TimerConfig(exec, true));
                } else {
                    executeAction(exec);
                }
            } else {
                logger.error("Rule Engine Input Message Bean got an unrecognized object (type: " +
                                omessage.getObject().getClass().getName() + ") - ignoring");
            }
        } catch (Exception ex) {
            logger.error("Exception in FactCollectionMessageBean", ex);
        }
    }

    @Timeout
    public void executeAction(Timer timer) {
        executeAction((ActionExecution) timer.getInfo());
    }

    /**
     * Executes the given action.
     * @param actionExec action to execute.
     */
    public void executeAction(ActionExecution actionExec) {
        logger.info("Got action exec with action=" + actionExec.getAction().getName() + " and trigger id = " + actionExec.getTrigger().getid());

        /* !!!! fake execution !!!! */
        try {
            Conclusion concl = conclCache.checkCache(actionExec.getTrigger().getConclusion());
            if ((concl != null) && !concl.isClosed()) { // execute action only if conclusion is still open
                actionExec.setTimeExecutedItem(new Date());
                Action action = actionExec.getAction();
                if (action instanceof CommandAction) {
                    if (((CommandAction) action).isCloseConclusionOnExecution()) {
                        concl.setClosed(true);
                        reDao.getEntityDao().mergeAndFlush(concl);
                        conclCache.removeFromCache(concl);
                        reManager.getEsperRuntime().sendEvent(concl);
                    }
                    logger.info("Executed COMMAND action. Action = " + actionExec.getAction().getName() + " and trigger id = " + actionExec.getTrigger().getid());
                }
            }

            // close the action in any case - if conclusion is open or closed
            actionExec.setTimeClosedItem(new Date());
            reDao.getEntityDao().merge(actionExec);
        } catch (Throwable th) {
            logger.error(th);
        }
        /****************************/

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
