/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 * This JSF bean is used to enable/disable rules in rules.xhtml page
 * @author evka
 */
@ManagedBean
@RequestScoped
public class EnableDisableRule extends JsfBeanBase {

    private Rule rule;

    /** Entity DAO. */
    @EJB
    private EntityDaoLocal entityDao;

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
        entityDao.merge(rule);
        return "";
    }

}
