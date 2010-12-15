/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import javax.ejb.EJB;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author evka
 */
@EJB(name="ejb/EntityDao", beanInterface=EntityDaoLocal.class)
public class AcknowledgeConclusion extends JsfBeanBase {

    private static final Logger logger = Logger.getLogger(AcknowledgeConclusion.class);

    private EntityDaoLocal entityDao;
    private Conclusion concl;

    public AcknowledgeConclusion() {
        super();
    }

    public void doAcknowledge() throws NamingException, PersistException, OnSaveProcessingException {
        logger.info("Acknowledging conclusion: " + concl.toString());
        if (concl == null) {
            return;
        }
        concl.setIsAcknowledged(true);
        getEntityDao().getPersistDao().merge(concl);
        return;
    }

    public Conclusion getConcl() {
        return concl;
    }

    public void setConcl(Conclusion concl) {
        this.concl = concl;
    }

    private EntityDaoLocal getEntityDao() throws NamingException {

        if (entityDao == null) {
            entityDao = (EntityDaoLocal) getEjb("ejb/EntityDao");
        }
        return entityDao;
    }

}
