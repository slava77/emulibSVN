/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.lang.reflect.InvocationTargetException;

/**
 * Objects of this class represent a property of an entity instance - can set and get it and also return metadata about it.
 * @author Evka
 */
public class EntityProperty {
    /** Reference to the entity instance. */
    private EntityBase entity;
    /** Property metadata. */
    private EntityPropertyMetadata metadata;

    public EntityProperty(EntityBase entity, EntityPropertyMetadata metadata) {
        this.entity = entity;
        this.metadata = metadata;
    }

    /**
     * Get the property value.
     * @return the property value.
     */
    public Object getProperty() throws IllegalAccessException, InvocationTargetException {
        return getMetadata().getGetterMethod().invoke(getEntity());
    }

    /**
     * Set the property value.
     * @param value the property value.
     */
    public void setProperty(Object value) throws IllegalAccessException, InvocationTargetException {
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
    public EntityPropertyMetadata getMetadata() {
        return metadata;
    }

    /**
     * Set property metadata.
     * @param metadata property metadata.
     */
    public void setMetadata(EntityPropertyMetadata metadata) {
        this.metadata = metadata;
    }
}
