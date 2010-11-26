/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.metadata;

import java.beans.PropertyDescriptor;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.List;
import java.util.logging.Logger;
import javax.persistence.ManyToMany;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class ManyToManyPropertyMd extends RestrictedPropertyMd {

    private static final Logger logger = Logger.getLogger(ManyToManyPropertyMd.class.getSimpleName());

    private static Class[] mandatoryAnnotations = {ManyToMany.class};

    public ManyToManyPropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);
        setIsMandatory(false);
    }

    @SuppressWarnings("unchecked")
    @Override
    public List<Object> getListOfValues(EntityDaoLocal entityDao) throws InvalidEntityClassException {
        Type gPropType = getGetterMethod().getGenericReturnType();
        if (!(gPropType instanceof ParameterizedType)) {
            throw new InvalidEntityClassException("ManyToManyPropertyMd cannot retrieve a list of values for property " + getName() + ", because the type of the property is not parameterized (posibly plain collection without the <T>)");
        }
        ParameterizedType pPropType = (ParameterizedType) gPropType;
        Type[] actualTypes = pPropType.getActualTypeArguments();
        if (actualTypes.length > 1) {
            throw new InvalidEntityClassException("ManyToManyPropertyMd cannot retrieve a list of values for property " + getName() + ", because the type of the property has more than one parameter");
        }
        if (actualTypes.length == 0) {
            throw new InvalidEntityClassException("ManyToManyPropertyMd cannot retrieve a list of values for property " + getName() + ", because the type of the property has zero parameters (posibly plain collection without the <T>)");
        }

        Class actualPropClass = (Class) actualTypes[0];

        return entityDao.getAllEntitiesByClass(actualPropClass);
    }
    
}
