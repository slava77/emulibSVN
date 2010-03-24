/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.FlushModeType;
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
    @EJB
    private EntityDaoLocal entityDao;

    public void persist(EntityBase cdwEntityObject) throws PersistException, Exception {

        // is it null by any chance? if yes - then be angry about it
        if (cdwEntityObject == null) {
            throw new Exception("null was passed to PersistDao.persist(EntityBase cdwEntityObject) method!");
        }

        // call an onSave trigger method to give the entity the last chance to prepare itself for persistance
        cdwEntityObject.onSave(entityDao);

        // persist the entity
        try {
            em.persist(cdwEntityObject);
        } catch (Exception ex) {
            String className = cdwEntityObject.getClass().getCanonicalName();
            logger.log(Level.SEVERE, "Error while persisting an entity of class " + className + ".\n toString(): " + cdwEntityObject.toString(), ex);

            throw new PersistException(ex);
        }
    }

}
