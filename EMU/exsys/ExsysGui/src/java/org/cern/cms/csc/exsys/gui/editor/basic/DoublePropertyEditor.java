/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.basic;

import javax.faces.convert.Converter;
import javax.faces.convert.DoubleConverter;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;

/**
 *
 * @author evka
 */
public class DoublePropertyEditor extends NumberPropertyEditor {

    public DoublePropertyEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
    }

    @Override
    public Class getValidValueClass() {
        return double.class;
    }

    @Override
    protected Converter createConverter() {
        return new DoubleConverter();
    }
}
