/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Local
public interface EntityDaoLocal {

    EntityBase getEntityById(final String entityClassName, String id) throws InvalidEntityClassException;
    <T extends EntityBase> T getEntityById(final Class<T> entityClass, final Object id) throws InvalidEntityClassException;
    <T extends EntityBase> List<T> getAllEntitiesByClass(final Class<T> entityClass) throws InvalidEntityClassException;

    EntityBase refreshEntity(EntityBase entity);
    EntityBase refreshEntity(EntityBase entity, boolean usingId);

    void persist(EntityBase cdwEntityObject) throws PersistException, OnSaveProcessingException;
    EntityBase persist(EntityBase cdwEntityObject, boolean queued, boolean useMerge) throws PersistException, OnSaveProcessingException;
    EntityBase merge(EntityBase cdwEntityObject) throws PersistException, OnSaveProcessingException;
    EntityBase mergeAndRefresh(EntityBase cdwEntityObject) throws PersistException, OnSaveProcessingException;
    void delete(EntityBase cdwEntityObject);

}
