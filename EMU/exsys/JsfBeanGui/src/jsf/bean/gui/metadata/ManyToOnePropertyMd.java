/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class ManyToOnePropertyMd extends RestrictedPropertyMd {

    private static Class[] mandatoryAnnotations = {ManyToOne.class, JoinColumn.class};

    public ManyToOnePropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);
        setIsMandatory(!joinColumn.nullable());
    }

}
