/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.metadata;

import java.beans.PropertyDescriptor;
import java.util.ArrayList;
import java.util.List;
import javax.persistence.Basic;
import javax.persistence.Column;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EnumPropertyMd extends RestrictedPropertyMd {

    public EnumPropertyMd(PropertyDescriptor prop, boolean isMandatory) throws InvalidEntityBeanPropertyException {
        super(prop);

        if (!prop.getPropertyType().isEnum()) {
            throw new InvalidEntityBeanPropertyException("EnumPropertyMd constructor - type of the property is not enum");
        }

        setIsMandatory(isMandatory);
    }

    @Override
    public List<Object> getListOfValues(EntityDaoLocal entityDao) {
        List<Object> ret = new ArrayList<Object>();
        Object[] values = getType().getEnumConstants();
        for (Object value: values) {
            ret.add(value);
        }
        return ret;
    }
}
