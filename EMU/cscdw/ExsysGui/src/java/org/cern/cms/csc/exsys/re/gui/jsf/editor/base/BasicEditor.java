/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.base;

import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.base.metadata.EntityBasicPropertyMD;
import org.cern.cms.csc.dw.model.base.metadata.EntityPropertyMD;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * Editor which is used to edit basic properties (String, Number, Date...)
 * @author Evka
 */
public abstract class BasicEditor extends Editor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public BasicEditor(EntityBase entity, EntityPropertyMD metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
        if (!(metadata instanceof EntityBasicPropertyMD)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a BasicEditor for a property which is not basic: " + metadata.getName());
        }
        if (! getValidValueClass().isAssignableFrom(metadata.getType())) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a " + this.getClass().getName() + " for a property with incompatible type." +
                                                         " Property name: " + metadata.getName() +
                                                         " Property type: " + metadata.getType().getName() +
                                                         " Valid property type for this editor is " + getValidValueClass().getName());
        }
    }

    /**
     * Get a class of values that can be handled by this editor.
     * @return a class of values that can be handled by this editor.
     */
    public abstract Class getValidValueClass();

    @Override
    @SuppressWarnings("unchecked")
    public EntityBasicPropertyMD getMetadata() {
        return (EntityBasicPropertyMD) super.getMetadata();
    }
}
