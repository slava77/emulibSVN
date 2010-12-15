/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf;

import java.util.List;
import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.dao.ActionDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Action;

/**
 *
 * @author Evka
 */

@EJB(name="ActionDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.ActionDaoLocal.class)
public class Actions extends JsfBeanBase {

    private ActionDaoLocal actionDao;

    /** Creates a new instance of RuleEngineFacade */
    public Actions() {
        super();
    }

    public List<Action> getAllActions() throws NamingException {
        return getActionDao().getAllActions();
    }

    private ActionDaoLocal getActionDao() throws NamingException {
        if (actionDao == null) {
            actionDao = (ActionDaoLocal) getEjb("ActionDaoRef");
        }
        return actionDao;
    }

}
