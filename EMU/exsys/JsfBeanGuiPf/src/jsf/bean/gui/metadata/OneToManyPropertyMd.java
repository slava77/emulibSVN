/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.lang.annotation.Annotation;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.List;
import javax.persistence.OneToMany;
import jsf.bean.gui.EntityBeanDaoIf;
import jsf.bean.gui.annotation.EntityTitle;
import jsf.bean.gui.annotation.EntityTitleType;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.exception.InvalidEntityClassException;

/**
 *
 * @author Evka
 */
public class OneToManyPropertyMd extends RestrictedPropertyMd {

    private static final EntityTitle DEFAULT_ENTITY_TITLE = new EntityTitle() {

        public EntityTitleType type() {
            return EntityTitleType.CONSTANT;
        }

        public String value() {
            return "{list}";
        }

        public Class<? extends Annotation> annotationType() {
            return EntityTitle.class;
        }
                    
    };

    private static final Class[] MANDATORY_ANNOTATIONS = { OneToMany.class };

    public OneToManyPropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, MANDATORY_ANNOTATIONS);
        setIsMandatory(false);
    }

    @SuppressWarnings("unchecked")
    @Override
    public List<Object> getListOfValues(EntityBeanDaoIf entityDao) throws InvalidEntityClassException {
        Type gPropType = getGetterMethod().getGenericReturnType();
        if (!(gPropType instanceof ParameterizedType)) {
            throw new InvalidEntityClassException("OneToManyPropertyMd cannot retrieve a list of values for property " + getName() + ", because the type of the property is not parameterized (posibly plain collection without the <T>)");
        }
        ParameterizedType pPropType = (ParameterizedType) gPropType;
        Type[] actualTypes = pPropType.getActualTypeArguments();
        if (actualTypes.length > 1) {
            throw new InvalidEntityClassException("OneToManyPropertyMd cannot retrieve a list of values for property " + getName() + ", because the type of the property has more than one parameter");
        }
        if (actualTypes.length == 0) {
            throw new InvalidEntityClassException("OneToManyPropertyMd cannot retrieve a list of values for property " + getName() + ", because the type of the property has zero parameters (posibly plain collection without the <T>)");
        }

        Class actualPropClass = (Class) actualTypes[0];

        return entityDao.getAllEntitiesByClass(actualPropClass);
    }

    @Override
    protected EntityTitle getDefaultEntityTitle() {
        return DEFAULT_ENTITY_TITLE;
    }
    
}
