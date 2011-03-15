/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.List;
import javax.ejb.Remote;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.exception.InvalidEntityClassException;

/**
 *
 * @author evka
 */
@Remote
public interface EntityDaoRemote {

    EntityBeanBase getEntityById(final String entityClassName, String id) throws InvalidEntityClassException;
    <T extends EntityBeanBase> T getEntityById(final Class<T> entityClass, final Object id) throws InvalidEntityClassException;
    <T extends EntityBeanBase> List<T> getAllEntitiesByClass(final Class<T> entityClass) throws InvalidEntityClassException;

    EntityBeanBase refreshEntity(EntityBeanBase entity);
    EntityBeanBase refreshEntity(EntityBeanBase entity, boolean usingId);

}
