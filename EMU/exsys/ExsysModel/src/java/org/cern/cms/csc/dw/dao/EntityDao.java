/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Stateless
public class EntityDao implements EntityDaoLocal {

    @EJB
    private PersistDaoLocal persistDao;

    @EJB
    private OntologyDaoLocal ontologyDao;

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    /**
     * Returns EntityBase given the class and ID
     * @param entityClass class of entity that you're looking for (must extend EntityBase)
     * @param id ID of the object that you're looking for
     * @return entity that you asked for or null if it doesn't exist
     * @throws InvalidEntityClassException this is thrown when the given class is not a subclass of EntityBase
     */
    public <T extends EntityBase> T getEntityById(final Class<T> entityClass, final Object id) throws InvalidEntityClassException {
        // check all the superclasses to see if it's a subclass of EntityBase, if not - throw InvalidEntityClassException
        boolean isEntity = EntityBase.class.isAssignableFrom(entityClass);
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
    public EntityBase getEntityById(final String entityClassName, String id) throws InvalidEntityClassException {
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
    public <T extends EntityBase> List<T> getAllEntitiesByClass(final Class<T> entityClass) throws InvalidEntityClassException {
        // check all the superclasses to see if it's a subclass of EntityBase, if not - throw InvalidEntityClassException
        boolean isEntity = EntityBase.class.isAssignableFrom(entityClass);
        if (!isEntity) {
            throw new InvalidEntityClassException(entityClass.getCanonicalName());
        }

        // get the entities using the EntityManager
        List<T> ret = em.createQuery("select ent from " + entityClass.getCanonicalName() + " as ent").getResultList();
        return ret;
    }

    /**
     * Get PersistDao Bean
     * @return PersistDao Bean
     */
    public PersistDaoLocal getPersistDao() {
        return persistDao;
    }

    /**
     * Ontology stuff
     * @return
     */
    public OntologyDaoLocal getOntologyDao() {
        return ontologyDao;
    }

    public EntityBase refreshEntity(EntityBase entity) {
        return refreshEntity(entity, true);
    }
    
    public EntityBase refreshEntity(EntityBase entity, boolean usingId) {
        if (usingId) {
            entity = em.getReference(entity.getClass(), entity.getEntityId());
            //entity = em.find(entity.getClass(), entity.getEntityId());
        } else {
            em.refresh(entity);
        }
        return entity;
    }

}
