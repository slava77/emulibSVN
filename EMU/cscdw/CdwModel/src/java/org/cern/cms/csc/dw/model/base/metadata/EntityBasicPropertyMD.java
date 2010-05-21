/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base.metadata;

import java.beans.PropertyDescriptor;
import javax.persistence.Basic;
import javax.persistence.Column;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EntityBasicPropertyMD extends EntityPropertyMD {

    private static Class[] mandatoryAnnotations = {Basic.class, Column.class};

    /** Value length limitation (e.g. for String values). */
    private int length = -1;

    public EntityBasicPropertyMD(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);
        
        Column column = prop.getReadMethod().getAnnotation(Column.class);
        setIsMandatory(!column.nullable());
        this.length = column.length();
    }

    /**
     * Get value length limitation (e.g. for String values).
     * @return value length limitation (e.g. for String values).
     */
    public int getLength() {
        return length;
    }

    /**
     * Validates a given value and returns an error message if the value is not valid, otherwise returns null.
     * @param value value to be validated
     * @return an error message if the value is not valid, otherwise - null is returned.
     */
    @Override
    public String validate(Object value) {
        // super validation
        String msg = super.validate(value);
        if (msg != null) {
            return msg;
        }

        // if this is a string value and type of the property is string, check if the value isn't too long
        if (value instanceof String) {
            String strValue = (String) value;
            if (getType().equals(String.class) && (getLength() > 0) && (strValue.length() > getLength())) {
                return "The value is too long (" + strValue.length() + " chars). Maximum number of characters: " + getLength();
            }
        }

        // validation successful
        return null;
    }
}
