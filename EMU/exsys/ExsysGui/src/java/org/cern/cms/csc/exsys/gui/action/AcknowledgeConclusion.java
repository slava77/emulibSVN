/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author evka
 */
@ManagedBean
@RequestScoped
public class AcknowledgeConclusion extends JsfBeanBase {

    private static final Logger logger = ExsysLogger.getLogger(AcknowledgeConclusion.class);

    @EJB
    private EntityDaoLocal entityDao;

    public AcknowledgeConclusion() {
        super();
    }

    public void setToAcknowledge(Conclusion conclusion) throws Exception {
        logger.info("Acknowledging conclusion: " + conclusion.toString());
        if (conclusion == null) {
            return;
        }
        conclusion.setAcknowledged(true);
        entityDao.merge(conclusion);
        return;
    }

}
