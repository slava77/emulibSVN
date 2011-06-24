/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.util.ArrayList;
import java.util.List;
import jsf.bean.gui.EntityBeanDaoIf;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.exception.InvalidEntityClassException;

/**
 *
 * @author Evka
 */
public abstract class RestrictedPropertyMd extends PropertyMd {

    private String referencedProperty = null;
    
    public RestrictedPropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop);
    }

    public RestrictedPropertyMd(PropertyDescriptor prop, Class[] mandatoryAnnotations) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);
    }

    @SuppressWarnings("unchecked")
    public List<Object> getListOfValues(EntityBeanDaoIf entityDao) throws InvalidEntityClassException {
        List<Object> ret = new ArrayList<Object>();
        if (!getIsMandatory()) {
            ret.add(null);
        }
        ret.addAll(entityDao.getAllEntitiesByClass(getType()));
        return ret;
    }

    public String getReferencedProperty() {
        return referencedProperty;
    }

    public void setReferencedProperty(String referencedProperty) {
        this.referencedProperty = referencedProperty;
    }
    
}
