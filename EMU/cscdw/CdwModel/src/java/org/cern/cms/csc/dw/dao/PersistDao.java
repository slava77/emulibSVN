/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.Session;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Stateless
public class PersistDao implements PersistDaoLocal {

    private static Logger logger = Logger.getLogger(PersistDao.class.getName());
    
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
        persist(cdwEntityObject, false);
    }

    public void persist(EntityBase cdwEntityObject, boolean queued) throws PersistException, OnSaveProcessingException {

        // is it null by any chance? if yes - then be angry about it
        if (cdwEntityObject == null) {
            throw new IllegalArgumentException("null was passed to PersistDao.persist(EntityBase cdwEntityObject) method!");
        }

        if (!queued && queueMode) {
            //entityToSaverQueue(cdwEntityObject);
            return;
        }

        // call an onSave trigger method to give the entity the last chance to prepare itself for persistance
        cdwEntityObject.onSave(entityDao, queued);

        // persist the entity
        try {
            em.persist(cdwEntityObject);
        } catch (Exception ex) {
            String className = cdwEntityObject.getClass().getCanonicalName();
            logger.log(Level.SEVERE, "Error while persisting an entity of class " + className + ".\n toString(): " + cdwEntityObject.toString(), ex);
            logger.log(Level.INFO, "Sending entity of class " + className + ".\n to saver queue...", ex);

            /* TODO:
             * put int object into queue. Still need to define how to handle queued objects, etc.
             * I.e. all objects (once in
             */
            // entityToSaverQueue(cdwEntityObject);
            // queueMode = true;

            throw new PersistException(ex);
        }
        
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
