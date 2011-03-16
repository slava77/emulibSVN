/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action.executor;

import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent.DataPropertyType;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.dw.util.ExsysIORemoteEjbLookup;
import org.cern.cms.csc.exsys.io.DimServiceProviderRemote;
import org.cern.cms.csc.exsys.re.action.ActionExecutor;
import org.cern.cms.csc.exsys.re.exception.ActionExecutionException;
import org.cern.cms.csc.exsys.re.model.ActionExecution;
import org.cern.cms.csc.exsys.re.model.DcsCommandAction;

/**
 *
 * @author evka
 */
public class DcsActionExecutor extends ActionExecutor {

    private static final String DELIMITER = "|";

    private EjbLookup<DimServiceProviderRemote> dimServiceProvider =
                      new ExsysIORemoteEjbLookup<DimServiceProviderRemote>(DimServiceProviderRemote.class);

    private EjbLookup<GOntologyDaoLocal> gOntologyDao = new EjbLookup<GOntologyDaoLocal>(GOntologyDaoLocal.class);

    public DcsActionExecutor(ActionExecution actionExec) {
        super(actionExec);
    }

    @Override
    protected void execute(ActionExecution actionExec) throws ActionExecutionException {
        try {
            StringBuilder dimData = new StringBuilder();

            // gather all data that we need
            DcsCommandAction dcsCommandAction = (DcsCommandAction) actionExec.getAction();
            Component comp = actionExec.getTrigger().getConclusion().getComponent();
            String dp = (String) gOntologyDao.ejb().getGComponent(comp).getDataProperty(DataPropertyType.DCS_ID);

            dimData.append(dcsCommandAction.getCommandType().value());
            dimData.append(DELIMITER);
            dimData.append(dp);
            dimData.append(DELIMITER);
            dimData.append(dcsCommandAction.getArgs().replaceAll(",", DELIMITER));

//            dimServiceProvider.ejb().publishString(dcsCommandAction, dimData.toString());
        } catch (Exception ex) {
            throw new ActionExecutionException("Could not execute DCS command", ex);
        }
    }

}
