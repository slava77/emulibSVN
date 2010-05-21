/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.base;

import java.lang.reflect.InvocationTargetException;
import java.util.List;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.base.metadata.EntityBasicPropertyMD;
import org.cern.cms.csc.dw.model.base.metadata.EntityPropertyMD;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.PropertyEditorFactory;

/**
 * Objects of this class represent an editor of an entity (subclass of EntityBase) and hold property editors for that entity object.
 * This is also a subclass of Editor so these objects are also used for object -> object relation editing (e.g. many-to-one relation).
 * @author Evka
 */
public abstract class EntityEditor extends Editor {

    /** Editors of the properties of the entity being edited by this EntityEditor. */
    private List<Editor> propertyEditors = null;

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public EntityEditor(EntityBase entity, EntityPropertyMD metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
        if ((metadata instanceof EntityBasicPropertyMD)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a " + this.getClass().getName() + " for a property which is basic: " + metadata.getName());
        }
    }

    /**
     * In addition to calling super.setValue(), this one resets the propertyEditors to null so that they'd be recreated when the
     * next getter call comes for that, because now that the value has changed - the property editors are no longer valid.
     * @see org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor.setValue()
     */
    @Override
    public void setValue(Object value) throws IllegalAccessException, InvocationTargetException {
        super.setValue(value);
        propertyEditors = null;
    }

    /**
     * Get editors of the properties of the entity being edited by this EntityEditor.
     * @return editors of the properties of the entity being edited by this EntityEditor.
     */
    public List<Editor> getPropertyEditors() {
        if (propertyEditors == null) {
            try {
                propertyEditors = PropertyEditorFactory.createPropertyEditors((EntityBase)getValue(), this);
            } catch (Exception ex) {
                throw new RuntimeException("Exception while trying to create property editors for an entity editor", ex);
            }
        }

        return propertyEditors;
    }

}
