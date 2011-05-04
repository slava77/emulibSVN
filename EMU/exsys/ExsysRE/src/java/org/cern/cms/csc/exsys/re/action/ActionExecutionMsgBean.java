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
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.util.ExceptionPrinter;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheServiceLocal;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.ActionExecution;
import org.cern.cms.csc.exsys.re.model.CommandAction;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 *
 * @author evka
 */
@MessageDriven(mappedName = "jms/ruleEngineActionQueue", activationConfig =  {
        @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge"),
        @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue")
    })
public class ActionExecutionMsgBean implements MessageListener {

    private static final Logger logger = ExsysLogger.getLogger(ActionExecutionMsgBean.class);

    @Resource
    private TimerService timerService;
    @EJB
    private ConclusionCacheServiceLocal conclCache;
    @EJB
    private RuleEngineDaoLocal reDao;
    @EJB
    private RuleEngineManagerLocal reManager;

    public ActionExecutionMsgBean() {
    }

    @Override
    public void onMessage(Message message) {
        try {
            ObjectMessage omessage = (ObjectMessage) message;
            if (omessage.getObject() instanceof ActionExecution) {
                ActionExecution exec = (ActionExecution) omessage.getObject();
                long timeout = exec.getAction().getTimeout();
                if (timeout > 0) {
                    logger.info("Registering action execution to be carried out in " + timeout + "ms. Action=" + exec.getAction().getName() + ", trigger id = " + exec.getTrigger().getid());
                    Timer timer = timerService.createSingleActionTimer(timeout, new TimerConfig(exec, true));
                } else {
                    executeAction(exec);
                }
            } else {
                logger.critical("Rule Engine Action Message Bean got an unrecognized object (type: " +
                                omessage.getObject().getClass().getName() + ") - ignoring");
            }
        } catch (Exception ex) {
            logger.critical("Exception in ActionExecutionMsgBean", ex);
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
        ConclusionType conclType = actionExec.getTrigger().getConclusion().getType();
        boolean isTransient = (conclType.isSetTransient() && conclType.isTransient());

        try {
            Conclusion concl = conclCache.checkCache(actionExec.getTrigger().getConclusion());
            if (((concl != null) && !concl.isClosed()) || isTransient) { // execute action only if conclusion is still open
                ActionExecutor executor = ActionExecutorFactory.createActionExecutor(actionExec);
                executor.execute();
                actionExec.setTimeExecuted(new Date());
                Action action = actionExec.getAction();
                if (action instanceof CommandAction) {
                    if (((CommandAction) action).isCloseConclusionOnExecution()) {
                        concl.setClosed(true);
                        if (!isTransient) {
                            reDao.getEntityDao().mergeAndFlush(concl);
                            conclCache.removeFromCache(concl);
                        }
                        reManager.postEvent(concl);
                    }
                    logger.info("Executed COMMAND action. Action = " + actionExec.getAction().getName() + " and trigger id = " + actionExec.getTrigger().getid());
                }
            } else if ((concl != null) && (concl.isClosed())) {
                logger.info("Action " + actionExec.getAction().getName() + " (trigger id=" + actionExec.getTrigger().getid() + ") is canceled because the corresponding conclusion is already closed");
            }

            // close the action in any case - if conclusion is open or closed
            actionExec.setTimeClosed(new Date());
            if (!isTransient) {
                reDao.getEntityDao().mergeAndFlush(actionExec);
            }
        } catch (Throwable th) {
            logger.critical("Error while executing action", th);
            try {
                actionExec.setError(ExceptionPrinter.printStackTraceToString(th));
                if (!isTransient) {
                    reDao.getEntityDao().mergeAndFlush(actionExec);
                }
            } catch (Throwable execSaveEx) {
                logger.critical("Error while trying to save action execution exception information", execSaveEx);
            }
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
