/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui;

import java.util.List;
import jsf.bean.gui.exception.InvalidEntityClassException;

/**
 *
 * @author Evka
 */
public interface EntityBeanDaoIf {

    EntityBeanBase getEntityById(final String entityClassName, String id) throws InvalidEntityClassException;
    <T extends EntityBeanBase> T getEntityById(final Class<T> entityClass, final Object id) throws InvalidEntityClassException;
    <T extends EntityBeanBase> List<T> getAllEntitiesByClass(final Class<T> entityClass) throws InvalidEntityClassException;

    EntityBeanBase refreshEntity(EntityBeanBase entity);
    EntityBeanBase refreshEntity(EntityBeanBase entity, boolean usingId);

//    EntityBeanBase persist(EntityBeanBase cdwEntityObject, boolean queued, boolean useMerge, boolean flush) throws Exception;
    void persist(EntityBeanBase cdwEntityObject) throws Exception;
    void persistAndFlush(EntityBeanBase cdwEntityObject) throws Exception;
    EntityBeanBase merge(EntityBeanBase cdwEntityObject) throws Exception;
    EntityBeanBase mergeAndFlush(EntityBeanBase cdwEntityObject) throws Exception;
    void delete(EntityBeanBase cdwEntityObject);

    void flush();
}
