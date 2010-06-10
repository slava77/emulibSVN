/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import java.math.BigInteger;
import java.util.Date;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.re.model.Rule;


/**
 *
 * @author Evka
 */

public class CreateRule extends EntityEditorManager {

    public CreateRule() {
        super(new Rule());
    }

    @Override
    public String doSave() throws PersistException, OnSaveProcessingException {
        Rule rule = (Rule) getTopEditor().getValue();
        BigInteger version = rule.getVersion();
        if (version == null) {
            rule.setVersion(BigInteger.ONE);
        } else {
            rule.setVersion(rule.getVersion().add(BigInteger.ONE));
        }
        
        getEntityDao().getPersistDao().merge(rule);
        return "navRules";
    }

}
