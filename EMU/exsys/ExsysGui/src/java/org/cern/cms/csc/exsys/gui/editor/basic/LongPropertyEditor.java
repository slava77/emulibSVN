/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.basic;

import javax.faces.convert.Converter;
import javax.faces.convert.DoubleConverter;
import javax.faces.convert.LongConverter;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;

/**
 *
 * @author evka
 */
public class LongPropertyEditor extends NumberPropertyEditor {

    public LongPropertyEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
    }

    @Override
    public Class getValidValueClass() {
        return long.class;
    }

    @Override
    protected Converter createConverter() {
        return new LongConverter();
    }
}
