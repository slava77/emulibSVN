/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import com.sun.faces.context.flash.ELFlash;
import java.io.Serializable;
import javax.ejb.EJB;
import javax.enterprise.context.RequestScoped;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.bean.ViewScoped;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;
import org.icefaces.bean.ViewRetained;
import org.icefaces.bean.WindowDisposed;


/**
 *
 * @author Evka
 */

@ManagedBean
@SessionScoped
public class EditRule extends EntityEditorManager implements Serializable {

    @EJB
    private RuleEngineDaoLocal reDao;

    @EJB
    private EditorDaoLocal editorDao;

    private RuleSet ruleSet;

    @Override
    protected EntityBase createEntity() {
//        if (ruleSet == null) {
//            throw new RuntimeException("RuleSet not provided");
//        }

        return new Rule();
    }

    @Override
    public String doSave() throws Exception {
        Rule rule = (Rule) getTopEditor().getValue();
        if (rule.getConclusionType().getRuleSet() == null) {
            rule.getConclusionType().setRuleSet(ruleSet);
        }
        if (!rule.getConclusionType().getRules().contains(rule)) { //desros vyniotinis :D
            rule.getConclusionType().getRules().add(rule);
        }
        rule.setEnabled(false);
        setToSave(rule);
        return "reRules";
    }

    @Override
    protected EditorDaoLocal getEditorDao() {
        return editorDao;
    }

    public void setToSave(Rule rule) throws Exception {
        reDao.saveRule(rule);
    }

    public void setToDelete(Rule rule) throws PersistException, OnSaveProcessingException {
        editorDao.getEntityDao().delete(rule);
    }

    public RuleSet getRuleSet() {
        return ruleSet;
    }

    public void setRuleSet(RuleSet ruleSet) {
        this.ruleSet = ruleSet;
    }

}
