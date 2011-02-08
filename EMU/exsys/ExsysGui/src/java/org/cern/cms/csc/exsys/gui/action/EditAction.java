/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.EmailAction;
import org.cern.cms.csc.exsys.re.model.SmsAction;


/**
 *
 * @author Evka
 */
@ManagedBean
@SessionScoped
public class EditAction extends EntityEditorManager {

    @EJB
    private EditorDaoLocal dao;

    public EditAction() {
        super();
    }

    @Override
    protected EntityBase createEntity() {
        return null;
    }

    public String createEmailAction() {
        setEntity(new EmailAction());
        return "reEditAction";
    }

    public String createSmsAction() {
        setEntity(new SmsAction());
        return "reEditAction";
    }

    @Override
    public String doSave() throws Exception {
        Action action = (Action) getTopEditor().getValue();
        getEditorDao().getEntityDao().merge(action);
        return "reActions";
    }

    @Override
    protected EditorDaoLocal getEditorDao() {
        return dao;
    }

}
