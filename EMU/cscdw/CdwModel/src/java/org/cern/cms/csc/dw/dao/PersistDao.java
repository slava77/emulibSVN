/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Stateless
public class PersistDao implements PersistDaoLocal {

    private static Logger logger = Logger.getLogger(PersistDao.class.getName());
    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    public void persist(EntityBase cdwEntityObject) throws PersistException, Exception {
        cdwEntityObject.prePersist();
        try {
            em.persist(cdwEntityObject);
        } catch (Exception ex) {
            String className = "null";
            if (cdwEntityObject != null) {
                className = cdwEntityObject.getClass().getCanonicalName();
            }
            logger.log(Level.SEVERE, "Error while persisting an entity of class " + className, ex);
            throw new PersistException(ex);
        }
    }

}
