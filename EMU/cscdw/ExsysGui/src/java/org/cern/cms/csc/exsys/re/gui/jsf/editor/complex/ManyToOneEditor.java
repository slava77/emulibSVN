/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import java.lang.reflect.InvocationTargetException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.metadata.EnumPropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.ManyToOnePropertyMd;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor.InputType;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.RestrictedEntityEditor;

/**
 * This type of editor is used for many-to-one relations between entities.
 * @author Evka
 */
public class ManyToOneEditor extends RestrictedEntityEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public ManyToOneEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, entityDao);
        if (!(metadata instanceof ManyToOnePropertyMd) && !(metadata instanceof EnumPropertyMd)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a ManyToOneEditor for a property which is not a many-to-one relation: " + metadata.getName());
        }
    }

    /**
     * Calls super.getValue() and if that comes back null then given that LOV (list of values) isn't null, returns the first LOV element.
     * @see org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor.setValue()
     */
    @Override
    public Object getValue() throws IllegalAccessException, InvocationTargetException {
        Object value = super.getValue();
        if ((value == null) && (getLovCache() != null)) {
            if (getLovCache().size() > 0) {
                return getLovCache().get(0);
            }
        }
        return value;
    }

    @Override
    protected InputType createInputType() {
        return InputType.SELECT_ONE_MENU;
    }

    /** Always returns false, telling the GUI to allow creation of new values. */
    @Override
    public boolean isRestrictionStrict() {
        return false;
    }

}
