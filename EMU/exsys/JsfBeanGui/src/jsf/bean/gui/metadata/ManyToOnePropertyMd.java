/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.lang.annotation.Annotation;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import jsf.bean.gui.annotation.EntityTitle;
import jsf.bean.gui.annotation.EntityTitleType;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class ManyToOnePropertyMd extends RestrictedPropertyMd {

    private static final EntityTitle DEFAULT_ENTITY_TITLE = new EntityTitle() {

        public EntityTitleType type() {
            return EntityTitleType.ENTITY;
        }

        public String value() {
            return "";
        }

        public Class<? extends Annotation> annotationType() {
            return EntityTitle.class;
        }
                    
    };
    
    private static final Class[] MANDATORY_ANNOTATIONS = {ManyToOne.class, JoinColumn.class};

    public ManyToOnePropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, MANDATORY_ANNOTATIONS);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);
        setIsMandatory(!joinColumn.nullable());
    }

    @Override
    protected EntityTitle getDefaultEntityTitle() {
        return DEFAULT_ENTITY_TITLE;
    }

}
