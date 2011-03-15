/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import java.io.Serializable;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.transaction.UserTransaction;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.DcsCommandAction;
import org.cern.cms.csc.exsys.re.model.EmailAction;
import org.cern.cms.csc.exsys.re.model.SmsAction;


/**
 *
 * @author Evka
 */
@ManagedBean
@SessionScoped
public class EditAction extends EntityEditorManager implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(EditAction.class);

    @EJB
    private EditorDaoLocal dao;
    @Resource
    private UserTransaction ut;

    private Class newActionClass;

    public EditAction() {
        super();
    }

    @Override
    protected EntityBase createEntity() throws Exception {
        if (newActionClass != null) {
            return (EntityBase) newActionClass.newInstance();
        } else {
            throw new RuntimeException("Couldn't create new action - newActionClass is not set");
        }
    }

    public void setNewActionClass(Class newActionClass) {
        this.newActionClass = newActionClass;
    }

    public void setActionToEdit(Action action) throws Exception {
        // reload the entity and load lazy relations
        ut.begin();
        action = (Action) dao.getEntityDao().refreshEntity(action);
        action.getConclusionTypes().size(); // load the lazy conclusionTypes relation
        ut.commit();
        setEntity(action);
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

    public void setToSave(Action action) throws Exception {
        dao.getEntityDao().merge(action);
    }

    public void setToDelete(Action action) {
        dao.getEntityDao().delete(action);
    }

}
