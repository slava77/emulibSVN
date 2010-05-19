/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.base;

import java.lang.reflect.InvocationTargetException;
import java.util.logging.Logger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.base.EntityPropertyMD;

/**
 * Objects of this class are used in GUI to setup an input for a specific entity property and actually set and get the value of that property.
 * @author Evka
 */
public class Editor {

    private static Logger logger = Logger.getLogger(Editor.class.getName());

    /** Reference to the entity instance. */
    private EntityBase entity;
    /** Property metadata. */
    private EntityPropertyMD metadata;

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     */
    public Editor(EntityBase entity, EntityPropertyMD metadata) {
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
     * Get property metadata.
     * @return property metadata.
     */
    public EntityPropertyMD getMetadata() {
        return metadata;
    }
}
