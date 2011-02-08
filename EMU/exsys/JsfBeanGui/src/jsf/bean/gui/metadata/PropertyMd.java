/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.annotation.ImmutableReference;
import jsf.bean.gui.annotation.Label;
import jsf.bean.gui.annotation.NoManualInput;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 * This class describes a property of an entity class (name, type, is mandatory, etc.).
 * All entities which are subclasses of EntityBase class can return a list of these objects corresponding to their attributes.
 * @author Evka
 */
public abstract class PropertyMd {

    private static Logger logger = SimpleLogger.getLogger(PropertyMd.class);

    private static Pattern camelCasePattern = Pattern.compile("(\\p{Upper}[\\p{Lower}\\d]*)");
    private static Pattern itemPropertyPattern = Pattern.compile("(.+)Item");

    /** Object describing the bean property (things like name, display name, setter, getter, etc.). */
    private PropertyDescriptor propertyDescriptor;
    /** Flag telling if the property is mandatory or optional. */
    private boolean isMandatory;
    /** Flag telling if manual input of this property is allowed or not. */
    private boolean isManualInputAllowed;
    /** Flag telling if a default value should be created
     * (this is determined from the annotation NoManualInput, so it can only be true if no manual inuput is allowed for this property) */
    private boolean createDefaultValue;
    /**
     * This is set to true is @ImmutableReference annotation is found on this property.
     * It only makes sense if this property is a reference to another entity and tells GUI not to allow the user to edit it.
     */
    private boolean isImmutableReference;
    /** Class of the entity whose property this object is describing. */
    private Class entityClass;

    @SuppressWarnings("unchecked")
    public PropertyMd(PropertyDescriptor prop, Class[] mandatoryAnnotations) throws InvalidEntityBeanPropertyException {
        this(prop);

        // check if all the annotations mentioned in mandatoryAnnotations list are present on the getter method
        Method getterMethod = prop.getReadMethod();
        for (Class annotation: mandatoryAnnotations) {
            if (getterMethod.getAnnotation(annotation) == null) {
                throw new InvalidEntityBeanPropertyException("Attempting to create an " + this.getClass().getName() + " for a property which doesn't have " + annotation.getName() + " annotation: " + getterMethod.toGenericString());
            }
        }
    }

    /**
     * Constructor.
     */
    @SuppressWarnings("unchecked")
    public PropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        prop.setDisplayName(nameToTitle(prop.getName()));
        prop.setShortDescription("");
        this.entityClass = prop.getReadMethod().getDeclaringClass();
        this.propertyDescriptor = prop;
        Field f = getField();
        isManualInputAllowed = true;
        if (f != null) {
            // check if it has a @NoManualInput annotation
            NoManualInput noManualInputA = f.getAnnotation(NoManualInput.class);
            if (noManualInputA != null) {
                isManualInputAllowed = false;
                createDefaultValue = noManualInputA.createDefaultValue();
            }

            // check if it has a @Label annotation
            Label labelA = f.getAnnotation(Label.class);
            if (labelA != null) {
                prop.setDisplayName(labelA.name());
                prop.setShortDescription(labelA.description());
            }

            // check if it has a @ImmutableReference annotation
            ImmutableReference immutableRefA = f.getAnnotation(ImmutableReference.class);
            if (immutableRefA != null) {
                isImmutableReference = true;
            }
        }
    }

    /**
     * Get a flag telling if a default value should be created
     * (this is determined from the annotation NoManualInput, so it can only be true if no manual inuput is allowed for this property)
     * @return a flag telling if a default value should be created.
     */
    public boolean getIsCreateDefaultValue() {
        return createDefaultValue;
    }

    private Field getField() {
        try {
            String fieldName = getPropertyDescriptor().getName();
            Matcher m = itemPropertyPattern.matcher(fieldName);
            if (m.matches()) {
                fieldName = m.group(1);
            }
            return getEntityClass().getDeclaredField(fieldName);
        } catch (Exception ex) {
            return null;
        }
    }

    /** Converts property name to title. */
    private String nameToTitle(String propName) {
        propName = propName.substring(0, 1).toUpperCase() + propName.substring(1); // upper case the first letter
        String ret = "";
        boolean firstMatch = true;
        String word = "";
        int retLength = 0;
        Matcher m = camelCasePattern.matcher(propName);
        while(m.find()) {
            word = m.group(1);
            if (!firstMatch) {
                ret += " ";
            } else {
                firstMatch = false;
            }
            retLength = ret.length();
            ret += word;
        }
        if (word.equals("Item")) {
            ret = ret.substring(0, retLength);
        }
        return ret;
    }

    /**
     * Get getter method.
     * @return getter method.
     */
    public Method getGetterMethod() {
        return propertyDescriptor.getReadMethod();
    }

    /**
     * Get flag telling if the property is mandatory or optional.
     * @return flag telling if the property is mandatory or optional.
     */
    public boolean getIsMandatory() {
        return isMandatory;
    }

    /**
     * Set flag telling if the property is mandatory or optional.
     * @param isMandatory flag telling if the property is mandatory or optional.
     */
    protected void setIsMandatory(boolean isMandatory) {
        this.isMandatory = isMandatory;
    }

    /**
     * Get name of the property.
     * @return name of the property.
     */
    public String getName() {
        return propertyDescriptor.getName();
    }

    /**
     * Get setter method.
     * @return setter method.
     */
    public Method getSetterMethod() {
        return propertyDescriptor.getWriteMethod();
    }


    /**
     * Get type of the property (java class).
     * @return type of the property (java class).
     */
    public Class getType() {
        return propertyDescriptor.getPropertyType();
    }


    /**
     * Get class of the entity whose property this object is describing.
     * @return class of the entity whose property this object is describing.
     */
    public Class getEntityClass() {
        return entityClass;
    }

    /**
     * Get human readable title of the property (one that can be put into GUI).
     * @return human readable title of the property (one that can be put into GUI).
     */
    public String getTitle() {
        return propertyDescriptor.getDisplayName();
    }

    /**
     * @return property description.
     */
    public String getDescription() {
        if ((propertyDescriptor.getShortDescription() == null) ||
             propertyDescriptor.getShortDescription().isEmpty()) {
            return null;
        }

        return propertyDescriptor.getShortDescription();
    }

    /**
     * Get object describing the bean property (things like name, display name, setter, getter, etc.).
     * @return object describing the bean property (things like name, display name, setter, getter, etc.).
     */
    public PropertyDescriptor getPropertyDescriptor() {
        return propertyDescriptor;
    }

    /**
     * Get a flag telling if manual input of this property is allowed or not.
     * @return a flag telling if manual input of this property is allowed or not.
     */
    public boolean getIsManualInputAllowed() {
        return isManualInputAllowed;
    }

    /** Get a flag telling if this property is a reference to an immutable object (which should not / could not be edited). */
    public boolean getIsImmutableReference() {
        return isImmutableReference;
    }

    /**
     * Get new value of the type appropriate for this type of property.
     * @return new value of the type appropriate for this type of property.
     */
    public Object getNewValue() throws InstantiationException, IllegalAccessException {
        return getType().newInstance();
    }

    /**
     * Validates a given value and returns an error message if the value is not valid, otherwise returns null.
     * @param value value to be validated
     * @return an error message if the value is not valid, otherwise - null is returned.
     */
    @SuppressWarnings("unchecked")
    public String validate(Object value) {
        // mandatory field == null ?
        if ((value == null) && getIsMandatory()) {
            return getTitle() + " is mandatory - value cannot be blank";
        }

        // is type of the value compatible with the property type? (skip primitive types, since e.g. boolean.class.isAssignableFrom(Boolean.class) returns false, which is not actually true.. also ignore when type is collection and type of the value is an array, since ManyToManyEditor converts collections to arrays (otherwise icefaces doesn't do conversion properly)
//        if ((value != null) && (!getType().isAssignableFrom(value.getClass()) && (!getType().isPrimitive()) &&
//                (!(Collection.class.isAssignableFrom(getType()) && value.getClass().isArray()) ))) {
//            String msgStr = "Wrong value type: expected " + getType().getName() + ", got " + value.getClass().getName();
//            logger.severe("Serious validation error for property " + getName() + " of class " + getGetterMethod().getDeclaringClass().getName() + ": " + msgStr);
//            return msgStr;
//        }

        // if the value is of type EntityBase, then run validation on all of it's properties and if that fails - reply that there's a validation error with the values properties
        // only do the recursive entity validation if this property is not a reference to an immutable object
        if ((value instanceof EntityBeanBase) && !getIsImmutableReference()) {
            EntityBeanBase entityValue = (EntityBeanBase) value;
            try {
                for (PropertyMd propMetadata: entityValue.getPropertyMetadata()) {
                    String propValidationMsg = propMetadata.validate(propMetadata.getGetterMethod().invoke(value));
                    if (propValidationMsg != null) {
                        return "This object has invalid properties";
                    }
                }
            } catch (Exception ex) {
                logger.error("Exception while validating a many-to-one relation value", ex);
                throw new RuntimeException("Exception while validating a many-to-one relation value", ex);
            }
        }

        // validation successful
        return null;
    }
}