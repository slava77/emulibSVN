/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.jms.JMSException;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Stateless
public class PersistDao implements PersistDaoLocal {

    private static Logger logger = SimpleLogger.getLogger(PersistDao.class);
    
    /** 
     * Indicates if objects need to be put into queue (to maintain processing order!)
     */
    private static boolean queueMode = false;

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;
    
    @EJB
    private EntityDaoLocal entityDao;

    //@Resource(name = "jms/entitySaverQueue")
    //private Queue entitySaverQueue;

    //@Resource(name = "jms/entitySaverQueueFactory")
    //private ConnectionFactory entitySaverQueueFactory;

    //private Connection entitySaverConnection;

    public PersistDao() throws JMSException {
        // entitySaverConnection = entitySaverQueueFactory.createConnection();
    }

    public void persist(EntityBase cdwEntityObject) throws PersistException, OnSaveProcessingException {
        persist(cdwEntityObject, false, false);
    }

    public EntityBase merge(EntityBase cdwEntityObject) throws PersistException, OnSaveProcessingException {
        return persist(cdwEntityObject, false, true);
    }

    public EntityBase mergeAndRefresh(EntityBase cdwEntityObject) throws PersistException, OnSaveProcessingException {
        persist(cdwEntityObject, false, true);
        //cdwEntityObject = em.merge(cdwEntityObject);
        return cdwEntityObject;
    }

    public EntityBase persist(EntityBase cdwEntityObject, boolean queued, boolean useMerge) throws PersistException, OnSaveProcessingException {

        // is it null by any chance? if yes - then be angry about it
        if (cdwEntityObject == null) {
            throw new IllegalArgumentException("null was passed to PersistDao.persist(EntityBase cdwEntityObject) method!");
        }

        if (!queued && queueMode) {
            //entityToSaverQueue(cdwEntityObject);
            return cdwEntityObject;
        }

        // call an onSave trigger method to give the entity the last chance to prepare itself for persistance
        cdwEntityObject.onSave(entityDao, queued);

        // persist the entity
        try {
            if (useMerge) {
                cdwEntityObject = em.merge(cdwEntityObject);
            } else {
                em.persist(cdwEntityObject);
            }
        } catch (Exception ex) {
            String className = cdwEntityObject.getClass().getCanonicalName();
            logger.error("Error while persisting an entity of class " + className + ".\n toString(): " + cdwEntityObject.toString(), ex);
            logger.info("Sending entity of class " + className + ".\n to saver queue...", ex);

            /* TODO:
             * put int object into queue. Still need to define how to handle queued objects, etc.
             * I.e. all objects (once in
             */
            // entityToSaverQueue(cdwEntityObject);
            // queueMode = true;

            throw new PersistException(ex);
        }

        return cdwEntityObject;
    }

    public void delete(EntityBase cdwEntityObject) {
        // refresh the entity and delete it
        cdwEntityObject = em.getReference(cdwEntityObject.getClass(), cdwEntityObject.getEntityId());
        em.remove(cdwEntityObject);
    }


    /*
    private Message createMessageForEntitySaverQueue(Session session, EntityBase entity) throws JMSException {
        ObjectMessage om = session.createObjectMessage();
        om.setObject(entity);
        return om;
    }

    private void entityToSaverQueue(EntityBase entity) throws JMSException {
        Session session = null;
        try {
            session = entitySaverConnection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            MessageProducer messageProducer = session.createProducer(entitySaverQueue);
            messageProducer.send(createMessageForEntitySaverQueue(session, entity));
        } finally {
            if (session != null) {
                try {
                    session.close();
                } catch (JMSException e) {
                    Logger.getLogger(this.getClass().getName()).log(Level.WARNING, "Cannot close session", e);
                }
            }
        }
    }
    */

}
