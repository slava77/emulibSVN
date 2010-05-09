/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.persistence.Basic;
import javax.persistence.Column;
import org.cern.cms.csc.dw.exception.NotImplementedException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * This class describes a property of an entity class (name, type, is mandatory, etc.).
 * All entities which are subclasses of EntityBase class can return a list of these objects corresponding to their attributes.
 * @author Evka
 */
public class EntityPropertyMetadata {

    private static Logger logger = Logger.getLogger(EntityPropertyMetadata.class.getName());

    public static Pattern getterPattern = Pattern.compile("(get|is)(\\p{Upper}.+)");
    private static Pattern camelCasePattern = Pattern.compile("(\\p{Upper}\\p{Lower}*)");

    /** Name of the property. */
    private String name;
    /** Human readable title of the property (one that can be put into GUI). */
    private String title;
    /** Flag telling if the property is mandatory or optional. */
    private boolean isMandatory;
    /** Type of the property (java class). */
    private Class type;
    /** Setter method. */
    private Method setterMethod;
    /** Getter method. */
    private Method getterMethod;
    /** Flag telling if the property is enumerated. */
    private boolean isEnum = false;
    /** Enum type (optional). */
    private Enum enumType = null;
    /** Value length limitation (e.g. for String values). */
    private int length = -1;

    /**
     * Constructor.
     */
    public EntityPropertyMetadata(Method getterMethod) throws InvalidEntityBeanPropertyException {
        Matcher m = getterPattern.matcher(getterMethod.getName());
        if (!m.matches()) {
            throw new InvalidEntityBeanPropertyException("Given getter name doesn't comply with a getter name convention: " + getterMethod.getName());
        }

        this.getterMethod = getterMethod;
        String propName = m.group(2);
        this.name = propName.substring(0, 1).toLowerCase() + propName.substring(1);
        this.title = nameToTitle(propName);
        this.type = getterMethod.getReturnType();
        Class entityClass = getterMethod.getDeclaringClass();
        try {
            Method setter = entityClass.getMethod("set" + propName, this.type);
            this.setterMethod = setter;
        } catch (NoSuchMethodException ex) {
            throw new InvalidEntityBeanPropertyException("Cannot find a setter method for property " + this.name + " in class " + entityClass.getName());
        }

        // process the annotations
        Basic basicA = getterMethod.getAnnotation(Basic.class);
        if (basicA != null) {
            Column column = getterMethod.getAnnotation(Column.class);
            if (column == null) {
                throw new InvalidEntityBeanPropertyException("Basic property " + entityClass.getName() + "." + this.name + "doesn't have a @Column annotation");
            }
            this.isMandatory = column.nullable();
            this.length = column.length();
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
     * Get flag telling if the property is enumerated.
     * @return flag telling if the property is enumerated.
     */
    public boolean getIsEnum() {
        return isEnum;
    }

    /**
     * Set flag telling if the property is enumerated.
     * @param isEnum flag telling if the property is enumerated.
     */
    public void setIsEnum(boolean isEnum) {
        this.isEnum = isEnum;
    }

    /**
     * Set Enum type (optional).
     * @param enumType Enum type.
     */
    public void setEnumType(Enum enumType) {
        this.enumType = enumType;
    }

    /**
     * Get valid enumeration values (if applicable, otherwise empty list is returned).
     * @return valid enumeration values.
     */
    public List<String> getEnumValues() {
        List<String> ret = new ArrayList<String>();
        if (!getIsEnum() || (enumType == null)) {
            return ret;
        }
        throw new NotImplementedException();
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
     * Get value length limitation (e.g. for String values).
     * @return value length limitation (e.g. for String values).
     */
    public int getLength() {
        return length;
    }

    /** Set value length limitation (e.g. for String values).
     * @param length value length limitation (e.g. for String values).
     */
    public void setLength(int length) {
        this.length = length;
    }

    public void validate(FacesContext context, UIComponent validate, Object value) {
        logger.info("validating value: " + value.toString());
        FacesMessage msg = null;
        if ((value == null) && getIsMandatory()) {
            msg = new FacesMessage("This field is mandatory - value cannot be blank");
        } else if (value instanceof String) {
            String strValue = (String) value;
            if (getType().equals(String.class) && (getLength() > 0) && (strValue.length() > getLength())) {
                msg = new FacesMessage("The value is too long (" + strValue.length() + " chars). Maximum number of characters: " + getLength());
            }
        }

        if (value.equals("bad")) {
            msg = new FacesMessage("Please don't enter \"bad\" :)");
        }

        logger.info("passed validation? " + (msg == null));
        if (msg != null) {
            ((UIInput)validate).setValid(false);
            context.addMessage(validate.getClientId(context), msg);
        }
    }
}
