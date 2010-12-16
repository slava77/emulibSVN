/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.metadata;

import java.beans.PropertyDescriptor;
import java.util.List;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public abstract class RestrictedPropertyMd extends PropertyMd {

    public RestrictedPropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop);
    }

    public RestrictedPropertyMd(PropertyDescriptor prop, Class[] mandatoryAnnotations) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);
    }

    @SuppressWarnings("unchecked")
    public List<Object> getListOfValues(EntityDaoLocal entityDao) throws InvalidEntityClassException {
        return entityDao.getAllEntitiesByClass(getType());
    }

}
