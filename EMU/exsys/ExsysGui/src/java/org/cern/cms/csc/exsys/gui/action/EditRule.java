/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import org.cern.cms.csc.dw.dao.GenericDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;


/**
 *
 * @author Evka
 */

@ManagedBean
@SessionScoped
public class EditRule extends EntityEditorManager {

    @EJB
    private RuleEngineDaoLocal reDao;

    @EJB
    private GenericDaoLocal genericDao;

    @Override
    protected EntityBase createEntity() {
        return new Rule();
    }

    @Override
    public String doSave() throws PersistException, OnSaveProcessingException {
        Rule rule = (Rule) getTopEditor().getValue();
        rule.setEnabled(false);
        setToSave(rule);
        return "reRules";
    }

    @Override
    protected GenericDaoLocal getGenericDao() {
        return genericDao;
    }

    public void setToSave(Rule rule) throws PersistException, OnSaveProcessingException {
        reDao.saveRule(rule);
    }

    public void setToDelete(Rule rule) throws PersistException, OnSaveProcessingException {
        genericDao.getEntityDao().delete(rule);
    }

}
