/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Local
public interface EntityDaoLocal {

    EntityBase getEntityById(final String entityClassName, String id) throws InvalidEntityClassException;
    <T extends EntityBase> T getEntityById(final Class<T> entityClass, final Object id) throws InvalidEntityClassException;
    
}
