/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 * This JSF bean is used to enable/disable rules in rules.xhtml page
 * @author evka
 */
@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class EnableDisableRule extends JsfBeanBase {

    private Rule rule;
    /** Entity DAO. */
    private EntityDaoLocal entityDao;

    public EnableDisableRule() {
        try {
            this.entityDao = (EntityDaoLocal) getEjb("ejb/EntityDao");
        } catch (NamingException nex) {
            throw new RuntimeException("Failed to instantiate EnableDisableRule - exception while looking up EntityDao EJB", nex);
        }
    }

    public Rule getRule() {
        return rule;
    }

    public void setRule(Rule rule) {
        this.rule = rule;
    }

    public boolean isEnabled() {
        return rule.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        rule.setEnabled(enabled);
    }

    public String save() throws Exception {
        entityDao.getPersistDao().merge(rule);
        return "";
    }

}
