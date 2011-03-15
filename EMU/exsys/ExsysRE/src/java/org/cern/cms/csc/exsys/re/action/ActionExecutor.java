/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action;

import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.exception.ActionExecutionException;
import org.cern.cms.csc.exsys.re.model.ActionExecution;

/**
 *
 * @author evka
 */
public abstract class ActionExecutor {

    private ActionExecution actionExec;

    public ActionExecutor(ActionExecution actionExec) {
        this.actionExec = actionExec;
    }

    public void execute() throws ActionExecutionException {
        execute(actionExec);
    }

    protected abstract void execute(ActionExecution actionExec) throws ActionExecutionException;

}
