/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import java.math.BigInteger;
import java.util.Date;
import javax.ejb.EJB;
import javax.naming.NamingException;
import javax.persistence.EntityManager;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.EntityEditor;
import org.cern.cms.csc.exsys.re.model.Rule;


/**
 *
 * @author Evka
 */

@EJB(name="ejb/RuleEngineDao", beanInterface=RuleEngineDaoLocal.class)
public class EditRule extends EntityEditorManager {

    private RuleEngineDaoLocal reDao;

    public EditRule() {
        super();
        try {
            reDao = (RuleEngineDaoLocal) getEjb("ejb/RuleEngineDao");
        } catch (NamingException nex) {
            throw new RuntimeException("Failed to instantiate EditRule - exception while looking up RuleEngineDao EJB", nex);
        }
    }

    @Override
    protected EntityBase createEntity() {
        return new Rule();
    }

    @Override
    public String doSave() throws PersistException, OnSaveProcessingException {
        Rule rule = (Rule) getTopEditor().getValue();
        BigInteger version = rule.getVersion();
        if (version == null || (reDao.getRulesByName(rule.getName()).size() == 0)) {
            rule.setVersion(BigInteger.ONE);
        } else {
            rule.setVersion(rule.getVersion().add(BigInteger.ONE));
        }
        rule.setid(null);
        rule.setEnabled(false);
        rule.getComponentFinder().setid(null);

        getEntityDao().getPersistDao().merge(rule);
        return "navRules";
    }

}
