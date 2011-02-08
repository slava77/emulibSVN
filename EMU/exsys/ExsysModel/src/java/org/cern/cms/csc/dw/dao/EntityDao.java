/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.exception.InvalidEntityClassException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Stateless
public class EntityDao implements EntityDaoLocal {

    private static Logger logger = SimpleLogger.getLogger(EntityDao.class);

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    /**
     * Returns EntityBase given the class and ID
     * @param entityClass class of entity that you're looking for (must extend EntityBase)
     * @param id ID of the object that you're looking for
     * @return entity that you asked for or null if it doesn't exist
     * @throws InvalidEntityClassException this is thrown when the given class is not a subclass of EntityBase
     */
    @Override
    public <T extends EntityBeanBase> T getEntityById(final Class<T> entityClass, final Object id) throws InvalidEntityClassException {
        // check all the superclasses to see if it's a subclass of EntityBase, if not - throw InvalidEntityClassException
        boolean isEntity = EntityBeanBase.class.isAssignableFrom(entityClass);
        if (!isEntity) {
            throw new InvalidEntityClassException(entityClass.getCanonicalName());
        }

        // get the entity using the EntityManager
        T ret = em.find(entityClass, id);
        return ret;
    }

    /**
     * Returns EntityBase given the class and ID. Here both parameters are string (class is then found and ID is casted to the right type)
     * @param entityClassName entityClass class of entity that you're looking for (must extend EntityBase)
     * @param id id ID of the object that you're looking for
     * @return entity that you asked for or null if it doesn't exist
     * @throws InvalidEntityClassException thrown in many cases e.g. class doesn't exist, doesnt extend EntityBase or ID method is not found or ID cast exception
     */
    @SuppressWarnings("unchecked")
    @Override
    public EntityBeanBase getEntityById(final String entityClassName, String id) throws InvalidEntityClassException {
        try {
            // get the class
            Class entityClass = Class.forName(entityClassName);

            // find out the class of the ID
            Class idClass = null;
            for (Method method : entityClass.getMethods()){
                if (method.isAnnotationPresent(javax.persistence.Id.class)) {
                    idClass = method.getReturnType();
                    break;
                }
            }
            // no ID found - exception
            if (idClass == null) {
                throw new InvalidEntityClassException(entityClass.getCanonicalName(), "This class doesn't have ID!");
            }

            // cast the ID that we've got here to the right class
            Object idObject = null;
            if (idClass.isAssignableFrom(String.class)) {
                idObject = id;
            } else if (Number.class.isAssignableFrom(idClass)) {
                Method valueOfMethod = idClass.getMethod("valueOf", String.class);
                idObject = valueOfMethod.invoke(null, id);
            }

            return getEntityById(entityClass, idObject);
        } catch (ClassNotFoundException cnfex) {
            throw new InvalidEntityClassException(cnfex);
        } catch (NoSuchMethodException noMethodEx) {
            throw new InvalidEntityClassException(noMethodEx);
        } catch (IllegalAccessException illegalAccessEx) {
            throw new InvalidEntityClassException(illegalAccessEx);
        } catch (InvocationTargetException invTargetEx) {
            throw new InvalidEntityClassException(invTargetEx);
        }
    }

    /**
     * Get all entities of the specified class
     * @param <T> Class of the entities you want to get
     * @param entityClass Class of the entities you want to get
     * @return A full list of entities of the given class
     * @throws InvalidEntityClassException this is thrown when the given class is not a subclass of EntityBase
     */
    @SuppressWarnings("unchecked")
    @Override
    public <T extends EntityBeanBase> List<T> getAllEntitiesByClass(final Class<T> entityClass) throws InvalidEntityClassException {
        // check all the superclasses to see if it's a subclass of EntityBeanBase, if not - throw InvalidEntityClassException
        boolean isEntity = EntityBeanBase.class.isAssignableFrom(entityClass);
        if (!isEntity) {
            throw new InvalidEntityClassException(entityClass.getCanonicalName());
        }

        // get the entities using the EntityManager
        List<T> ret = em.createQuery("select ent from " + entityClass.getCanonicalName() + " as ent").getResultList();
        return ret;
    }

    @Override
    public EntityBeanBase refreshEntity(EntityBeanBase entity) {
        return refreshEntity(entity, true);
    }
    
    @Override
    public EntityBeanBase refreshEntity(EntityBeanBase entity, boolean usingId) {
        if (usingId) {
            entity = em.getReference(entity.getClass(), entity.getEntityId());
            //entity = em.find(entity.getClass(), entity.getEntityId());
        } else {
            em.refresh(entity);
        }
        return entity;
    }

    @Override
    public void persist(EntityBeanBase cdwEntityObject) throws PersistException, OnSaveProcessingException {
        persist(cdwEntityObject, false, false);
    }

    @Override
    public EntityBeanBase merge(EntityBeanBase cdwEntityObject) throws PersistException, OnSaveProcessingException {
        return persist(cdwEntityObject, false, true);
    }

    @Override
    public EntityBeanBase mergeAndRefresh(EntityBeanBase cdwEntityObject) throws PersistException, OnSaveProcessingException {
        persist(cdwEntityObject, false, true);
        //cdwEntityObject = em.merge(cdwEntityObject);
        return cdwEntityObject;
    }

    @Override
    public EntityBeanBase persist(EntityBeanBase cdwEntityObject, boolean queued, boolean useMerge) throws PersistException, OnSaveProcessingException {

        // is it null by any chance? if yes - then be angry about it
        if (cdwEntityObject == null) {
            throw new IllegalArgumentException("null was passed to PersistDao.persist(EntityBeanBase cdwEntityObject) method!");
        }

        // call an onSave trigger method to give the entity the last chance to prepare itself for persistance
        ((EntityBase) cdwEntityObject).onSave(this, queued);

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

    @Override
    public void delete(EntityBeanBase cdwEntityObject) {
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
