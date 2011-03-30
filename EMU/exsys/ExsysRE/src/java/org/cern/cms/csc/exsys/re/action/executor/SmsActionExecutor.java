/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action.executor;

import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.util.SmsSender;
import org.cern.cms.csc.exsys.re.action.ActionExecutor;
import org.cern.cms.csc.exsys.re.exception.ActionExecutionException;
import org.cern.cms.csc.exsys.re.model.ActionExecution;
import org.cern.cms.csc.exsys.re.model.SmsAction;

/**
 *
 * @author evka
 */
public class SmsActionExecutor extends ActionExecutor {

    private static final Logger logger = ExsysLogger.getLogger(EmailActionExecutor.class);

    public SmsActionExecutor(ActionExecution actionExec) {
        super(actionExec);
    }

    @Override
    protected void execute(ActionExecution actionExec) throws ActionExecutionException {
        try {
            SmsAction smsAction = (SmsAction) actionExec.getAction();
            logger.info("Sending SMS to " + smsAction.getRecipients() +
                       " (action name = \"" + smsAction.getName() +
                       "\",action execution ID=" + actionExec.getid() + ")");
            SmsSender.send(smsAction.getRecipients(),
                    actionExec.getTrigger().getConclusion().getTitle(),
                    actionExec.getTrigger().getType().toString());
        } catch (Exception ex) {
            throw new ActionExecutionException("Could not send SMS", ex);
        }
    }

}
