/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.basic;

import java.lang.reflect.InvocationTargetException;
import javax.faces.convert.Converter;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.BasicEditor;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;

/**
 * Editor used to edit string properties
 * @author Evka
 */
public class StringPropertyEditor extends BasicEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public StringPropertyEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
    }

    @Override
    public void setValue(Object value) throws IllegalAccessException, InvocationTargetException {
        if (((String) value).isEmpty()) {
            value = null;
        }
        super.setValue(value);
    }

    @Override
    public Class getValidValueClass() {
        return String.class;
    }

    @Override
    protected Converter createConverter() {
        return null;
    }

    @Override
    protected InputType createInputType() {
        int maxLength = getMetadata().getLength();
        if (maxLength > 512) {
            return InputType.TEXT_AREA;
        }
        return InputType.DEFAULT;
    }

}
