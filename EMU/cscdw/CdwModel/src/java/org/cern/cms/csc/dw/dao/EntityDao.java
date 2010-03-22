/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
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

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    /**
     * Returns EntityBase given the class and ID
     * @param entityClass class of entity that you're looking for (must extend EntityBase)
     * @param id ID of the object that you're looking for
     * @return entity that you asked for or null if it doesn't exist
     * @throws InvalidEntityClassException this is thrown when the given class is not a subclass of EntityBase
     */
    public EntityBase getEntityById(final Class entityClass, final Object id) throws InvalidEntityClassException {
        // check all the superclasses to see if it's a subclass of EntityBase, if not - throw InvalidEntityClassException
        boolean isEntity = EntityBase.class.isAssignableFrom(entityClass);
        if (!isEntity) {
            throw new InvalidEntityClassException(entityClass.getCanonicalName());
        }

        // get the entity using the EntityManager
        Object ret = em.find(entityClass, id);
        if (ret instanceof EntityBase) {
            return (EntityBase) ret;
        }
        return null;
    }

    /**
     * Returns EntityBase given the class and ID. Here both parameters are string (class is then found and ID is casted to the right type)
     * @param entityClassName entityClass class of entity that you're looking for (must extend EntityBase)
     * @param id id ID of the object that you're looking for
     * @return entity that you asked for or null if it doesn't exist
     * @throws InvalidEntityClassException thrown in many cases e.g. class doesn't exist, doesnt extend EntityBase or ID method is not found or ID cast exception
     */
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
 
}
