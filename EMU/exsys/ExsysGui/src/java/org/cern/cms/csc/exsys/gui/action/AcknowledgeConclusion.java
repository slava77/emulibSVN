/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author evka
 */
@ManagedBean
@RequestScoped
public class AcknowledgeConclusion extends JsfBeanBase {

    private static final Logger logger = SimpleLogger.getLogger(AcknowledgeConclusion.class);

    @EJB
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
        entityDao.merge(concl);
        return;
    }

    public Conclusion getConcl() {
        return concl;
    }

    public void setConcl(Conclusion concl) {
        this.concl = concl;
    }

}
