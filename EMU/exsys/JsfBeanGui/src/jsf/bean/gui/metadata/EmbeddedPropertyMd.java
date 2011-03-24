/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.util.List;
import javax.persistence.Embedded;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.exception.InvalidEntityClassException;

/**
 *
 * @author Evka
 */
public class EmbeddedPropertyMd extends PropertyMd {

    private static Class[] mandatoryAnnotations = { Embedded.class };

    private final Class type;
    private final List<PropertyMd> properties;

    public EmbeddedPropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);
        try {
            this.type = prop.getPropertyType();
            this.properties = EntityPropertyMdFactory.createMetadataForEntity(this.type);
            for (PropertyMd pmd : this.properties) {
                if (pmd.getIsMandatory()) {
                    setIsMandatory(true);
                    break;
                }
            }
        } catch (InvalidEntityClassException ex) {
            throw new InvalidEntityBeanPropertyException("Exception while creating Property metadata for embedded object property", ex);
        }
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

        if (value != null) {
            for (PropertyMd pmd: properties) {
                msg = pmd.validate(pmd.getPropertyValue(value));
                if (msg != null) {
                    return msg;
                }
            }
        }

        // validation successful
        return null;
    }

    public List<PropertyMd> getProperties() {
        return properties;
    }

}
