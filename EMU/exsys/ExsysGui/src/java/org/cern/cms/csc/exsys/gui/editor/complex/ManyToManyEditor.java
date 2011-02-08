/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.complex;

import java.lang.reflect.InvocationTargetException;
import java.util.Arrays;
import java.util.Collection;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.metadata.ManyToManyPropertyMd;
import jsf.bean.gui.metadata.PropertyMd;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;
import org.cern.cms.csc.exsys.gui.editor.base.RestrictedEntityEditor;

/**
 * This type of editor is used for many-to-one relations between entities.
 * @author Evka
 */
public class ManyToManyEditor extends RestrictedEntityEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public ManyToManyEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EditorDaoLocal EditorDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, EditorDao);
        if (!(metadata instanceof ManyToManyPropertyMd)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a ManyToManyEditor for a property which is not a many-to-many relation: " + metadata.getName());
        }
    }

    public Object[] getValueAsArray() throws IllegalAccessException, InvocationTargetException {
        Object value = super.getValue();
        if (value.getClass().isArray()) {
            return (Object[]) value;
        }
        if (Collection.class.isAssignableFrom(value.getClass())) {
            Collection col = (Collection) value;
            return col.toArray();
        }
        return new Object[]{};
    }

    public void setValueAsArray(Object[] value) throws IllegalAccessException, InvocationTargetException {
        if (Collection.class.isAssignableFrom(getMetadata().getType())) {
            super.setValue(Arrays.asList(value));
        } else if (getMetadata().getType().isArray()) {
            super.setValue(value);
        }
    }

    @Override
    protected InputType createInputType() {
        return InputType.SELECT_MANY_LIST;
    }

    @Override
    public boolean isRestrictionStrict() {
        return false;
    }

}
