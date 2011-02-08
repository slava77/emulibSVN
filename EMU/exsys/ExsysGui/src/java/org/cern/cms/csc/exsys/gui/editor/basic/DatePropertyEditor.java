/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.basic;

import java.util.Date;
import javax.faces.convert.Converter;
import javax.faces.convert.DateTimeConverter;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.base.BasicEditor;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;

/**
 * Editor used to edit Date properties
 * @author Evka
 */
public class DatePropertyEditor extends BasicEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public DatePropertyEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
    }

    @Override
    public Class getValidValueClass() {
        return Date.class;
    }

    @Override
    protected Converter createConverter() {
        return new DateTimeConverter();
    }

    @Override
    protected InputType createInputType() {
        return InputType.DATE_PICKER;
    }

}
