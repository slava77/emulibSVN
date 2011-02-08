/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import jsf.bean.gui.EntityBeanDaoIf;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;

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
    public List<Object> getListOfValues(EntityBeanDaoIf entityDao) {
        List<Object> ret = new ArrayList<Object>();
        Object[] values = getType().getEnumConstants();
        ret.addAll(Arrays.asList(values));
        return ret;
    }
}
