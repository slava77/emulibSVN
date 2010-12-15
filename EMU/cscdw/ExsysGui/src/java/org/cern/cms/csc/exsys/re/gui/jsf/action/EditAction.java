/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.EmailAction;
import org.cern.cms.csc.exsys.re.model.SmsAction;


/**
 *
 * @author Evka
 */

public class EditAction extends EntityEditorManager {

    public EditAction() {
        super();
    }

    @Override
    protected EntityBase createEntity() {
        return null;
    }

    public String createEmailAction() {
        setEntity(new EmailAction());
        return "newAction";
    }

    public String createSmsAction() {
        setEntity(new SmsAction());
        return "newAction";
    }

    @Override
    public String doSave() throws PersistException, OnSaveProcessingException {
        Action action = (Action) getTopEditor().getValue();

        getEntityDao().getPersistDao().merge(action);
        return "navActions";
    }

}
