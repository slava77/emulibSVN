/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.lang.reflect.Method;
import java.util.List;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.apache.commons.beanutils.BeanUtils;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * This class describes a property of an entity class (name, type, is mandatory, etc.).
 * All entities which are subclasses of EntityBase class can return a list of these objects corresponding to their attributes.
 * @author Evka
 */
public abstract class EntityPropertyMD {

    public enum PropertyRelationType {
        BASIC,
        MANY_TO_ONE,
        MANY_TO_MANY,
        ONE_TO_ONE,
        ONE_TO_MANY
    }

    private static Logger logger = Logger.getLogger(EntityPropertyMD.class.getName());

    public static Pattern getterPattern = Pattern.compile("(get|is)(\\p{Upper}.+)");
    private static Pattern camelCasePattern = Pattern.compile("(\\p{Upper}\\p{Lower}*)");

    /** Name of the property. */
    private String name;
    /** Human readable title of the property (one that can be put into GUI). */
    private String title;
    /** Flag telling if the property is mandatory or optional. */
    private boolean isMandatory;
    /** Class of the entity whose property this object is describing. */
    private Class entityClass;
    /** Type of the property (java class). */
    private Class type;
    /** Setter method. */
    private Method setterMethod;
    /** Getter method. */
    private Method getterMethod;
    /** This is used to supply info and perform certain tasks for GUI (like tell what kind of input to use, validate user input, supply a value converter). */
    private EntityPropertyGuiHandler guiHandler;
    /** Input type - used by GUI to decide on what type of input component to render. */
    private String inputType = "default";


    @SuppressWarnings("unchecked")
    public EntityPropertyMD(Method getterMethod, Class[] mandatoryAnnotations) throws InvalidEntityBeanPropertyException {
        this(getterMethod);

        // check if all the annotations mentioned in mandatoryAnnotations list are present on the getter method
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
    public EntityPropertyMD(Method getterMethod) throws InvalidEntityBeanPropertyException {
        Matcher m = getterPattern.matcher(getterMethod.getName());
        if (!m.matches()) {
            throw new InvalidEntityBeanPropertyException("Given getter name doesn't comply with a getter name convention: " + getterMethod.getName());
        }

        this.getterMethod = getterMethod;
        String propName = m.group(2);
        this.name = propName.substring(0, 1).toLowerCase() + propName.substring(1);
        this.title = nameToTitle(propName);
        this.type = getterMethod.getReturnType();
        this.entityClass = getterMethod.getDeclaringClass();
        try {
            Method setter = entityClass.getMethod("set" + propName, this.type);
            this.setterMethod = setter;
        } catch (NoSuchMethodException ex) {
            throw new InvalidEntityBeanPropertyException("Cannot find a setter method for property " + this.name + " in class " + entityClass.getName());
        }
    }

    /** Converts property name to title. */
    private String nameToTitle(String propName) {
        String ret = "";
        boolean firstMatch = true;
        String word = null;
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
        return getterMethod;
    }

    /**
     * Set getter method.
     * @param getterMethod getter method.
     */
    public void setGetterMethod(Method getterMethod) {
        this.getterMethod = getterMethod;
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
    public void setIsMandatory(boolean isMandatory) {
        this.isMandatory = isMandatory;
    }

    /**
     * Get name of the property.
     * @return name of the property.
     */
    public String getName() {
        return name;
    }

    /**
     * Set name of the property.
     * @param name name of the property.
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Get setter method.
     * @return setter method.
     */
    public Method getSetterMethod() {
        return setterMethod;
    }

    /**
     * Set setter method.
     * @param setterMethod setter method.
     */
    public void setSetterMethod(Method setterMethod) {
        this.setterMethod = setterMethod;
    }

    /**
     * Get type of the property (java class).
     * @return type of the property (java class).
     */
    public Class getType() {
        return type;
    }

    /**
     * Set type of the property (java class).
     * @param type type of the property (java class).
     */
    public void setType(Class type) {
        this.type = type;
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
        return title;
    }

    /**
     * Set human readable title of the property (one that can be put into GUI).
     * @param title human readable title of the property (one that can be put into GUI).
     */
    public void setTitle(String title) {
        this.title = title;
    }

    /**
     * Get relation type (e.g. basic, one-to-one, many-to-one, one-to-many, many-to-many).
     * @return relation type (e.g. basic, one-to-one, many-to-one, one-to-many, many-to-many).
     */
    public abstract PropertyRelationType getRelationType();

    /**
     * Get flag telling if this property is actually a collection.
     * @return flag telling if this property is actually a collection.
     */
    public abstract boolean getIsCollection();

    /**
     * Get flag telling if the type is basic java type (e.g. boolean, String, Integer...) or a pointer to another class.
     * @return flag telling if the type is basic java type (e.g. boolean, String, Integer...) or a pointer to another class.
     */
    public abstract boolean getIsTypeBasic();

    /**
     * Get list of available values. By default returns null. Applicable for e.g. enum or many-to-one type properties
     * @return
     */
    public List<Object> getListOfValues() throws Exception {
        return null;
    }

    /**
     * Get input type - used by GUI to decide on what type of input component to render.
     * @return input type - used by GUI to decide on what type of input component to render.
     */
    public String getInputType() {
        return inputType;
    }

    /**
     * Set input type - used by GUI to decide on what type of input component to render.
     * @param inputType input type - used by GUI to decide on what type of input component to render.
     */
    public void setInputType(String inputType) {
        this.inputType = inputType;
    }

    /**
     * Get this is used to supply info and perform certain tasks for GUI (like tell what kind of input to use, validate user input, supply a value converter).
     * @return this is used to supply info and perform certain tasks for GUI (like tell what kind of input to use, validate user input, supply a value converter).
     */
    public EntityPropertyGuiHandler getGuiHandler() {
        if (guiHandler == null) {
            guiHandler = new EntityPropertyGuiHandler(this);
        }
        return guiHandler;
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
            return "This field is mandatory - value cannot be blank";
        }

        // is type of the value compatible with the property type?
        if (!getType().isAssignableFrom(value.getClass())) {
            String msgStr = "Wrong value type: expected " + getType().getName() + ", got " + value.getClass().getName();
            logger.severe("Serious validation error for property " + getName() + " of class " + getGetterMethod().getDeclaringClass().getName() + ": " + msgStr);
            return msgStr;
        }

        // validation successful
        return null;
    }
}
