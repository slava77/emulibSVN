/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import org.cern.cms.csc.dw.gui.jsf.EntityPropertyEditor;
import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.logging.Logger;

/**
 * Objects of this class represent a property of an entity instance - can set and get it and also return metadata about it.
 * @author Evka
 */
public class EntityProperty {

    private static Logger logger = Logger.getLogger(EntityProperty.class.getName());

    /** Reference to the entity instance. */
    private EntityBase entity;
    /** Property metadata. */
    private EntityPropertyMD metadata;
    /** This is used to supply info and perform certain tasks for GUI (like tell what kind of input to use, validate user input, supply a value converter). */
    private EntityPropertyEditor editor;

    public EntityProperty(EntityBase entity, EntityPropertyMD metadata) {
        this.entity = entity;
        this.metadata = metadata;
    }

    /**
     * Get the property value.
     * @return the property value.
     */
    public Object getValue() throws IllegalAccessException, InvocationTargetException {
        return getMetadata().getGetterMethod().invoke(getEntity());
    }

    /**
     * Set the property value.
     * @param value the property value.
     */
    public void setValue(Object value) throws IllegalAccessException, InvocationTargetException {
        if (value instanceof String) {
            if (((String) value).isEmpty()) {
                logger.info("Value is ignored because it's empty string");
                value = null;
            }
        }

        if (value != null) {
            logger.info("Setting " + value.toString() + " for property " + getMetadata().getName());
        }
        getMetadata().getSetterMethod().invoke(getEntity(), value);
    }

    /**
     * Get reference to the entity instance.
     * @return reference to the entity instance.
     */
    public EntityBase getEntity() {
        return entity;
    }

    /**
     * Set reference to the entity instance.
     * @param entity reference to the entity instance.
     */
    public void setEntity(EntityBase entity) {
        this.entity = entity;
    }

    /**
     * Get property metadata.
     * @return property metadata.
     */
    public EntityPropertyMD getMetadata() {
        return metadata;
    }

    /**
     * Set property metadata.
     * @param metadata property metadata.
     */
    public void setMetadata(EntityPropertyMD metadata) {
        this.metadata = metadata;
    }

    /**
     * Get an object which is used to supply info and perform certain tasks for GUI (like tell what kind of input to use, validate user input, supply a value converter).
     * @return an object which is used to supply info and perform certain tasks for GUI (like tell what kind of input to use, validate user input, supply a value converter).
     */
    public EntityPropertyEditor getEditor() {
        if (editor == null) {
            editor = new EntityPropertyEditor(getMetadata());
        }
        return editor;
    }
}
