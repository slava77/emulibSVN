/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import java.io.Serializable;
import java.math.BigInteger;
import java.util.ArrayList;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.bean.ViewScoped;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.RuleSet;
import org.icefaces.bean.ViewRetained;
import org.icefaces.bean.WindowDisposed;

/**
 *
 * @author evka
 */
@ManagedBean
@SessionScoped
public class EditRuleSet extends EntityEditorManager implements Serializable {

    @EJB
    private EditorDaoLocal editorDao;

    /** Creates a new instance of EditRuleSet */
    public EditRuleSet() {
    }

    @Override
    protected EntityBase createEntity() throws Exception {
        return new RuleSet();
    }

    @Override
    public String doSave() throws Exception {
        RuleSet ruleSet = (RuleSet) getTopEditor().getValue();
        if (ruleSet.getConclusionTypes() == null) {
            ruleSet.setConclusionTypes(new ArrayList<ConclusionType>());
        }
        if (ruleSet.getVersion() == null) {
            ruleSet.setVersion(BigInteger.ONE);
        }
        setToSave(ruleSet);
        return "reRuleSets";
    }

    @Override
    protected EditorDaoLocal getEditorDao() {
        return editorDao;
    }

    public void setToSave(RuleSet ruleSet) throws Exception {
        editorDao.getEntityDao().merge(ruleSet);
    }

    public void setToDelete(RuleSet ruleSet) throws PersistException, OnSaveProcessingException {
        editorDao.getEntityDao().delete(ruleSet);
    }

}
