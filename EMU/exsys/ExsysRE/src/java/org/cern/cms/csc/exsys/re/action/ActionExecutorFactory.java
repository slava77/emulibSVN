/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action;

import org.cern.cms.csc.exsys.re.action.executor.DcsActionExecutor;
import org.cern.cms.csc.exsys.re.action.executor.EmailActionExecutor;
import org.cern.cms.csc.exsys.re.action.executor.SmsActionExecutor;
import org.cern.cms.csc.exsys.re.exception.ActionExecutionException;
import org.cern.cms.csc.exsys.re.model.ActionExecution;
import org.cern.cms.csc.exsys.re.model.DcsCommandAction;
import org.cern.cms.csc.exsys.re.model.EmailAction;
import org.cern.cms.csc.exsys.re.model.SmsAction;

/**
 *
 * @author evka
 */
public class ActionExecutorFactory {

    public static ActionExecutor createActionExecutor(ActionExecution actionExec) throws ActionExecutionException {
        if (actionExec.getAction() instanceof EmailAction) {
            return new EmailActionExecutor(actionExec);
        } else if (actionExec.getAction() instanceof SmsAction) {
            return new SmsActionExecutor(actionExec);
        } else if (actionExec.getAction() instanceof DcsCommandAction) {
            return new DcsActionExecutor(actionExec);
        }
        throw new ActionExecutionException("Action type " + actionExec.getAction().getClass().getSimpleName() + " is not yet supported.");
    }

}
