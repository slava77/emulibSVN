/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.basic;

import java.math.BigInteger;
import javax.faces.convert.BigIntegerConverter;
import javax.faces.convert.Converter;
import javax.faces.convert.DoubleConverter;
import javax.faces.convert.FloatConverter;
import javax.faces.convert.NumberConverter;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.BasicEditor;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;

/**
 * Editor used to edit number properties
 * @author Evka
 */
public class NumberPropertyEditor extends BasicEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public NumberPropertyEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
    }

    @Override
    public Class getValidValueClass() {
        return Number.class;
    }

    @Override
    protected Converter createConverter() {
        if (BigInteger.class.isAssignableFrom(getMetadata().getType())) { // it's an integer
            return new BigIntegerConverter();
        } else if (Float.class.isAssignableFrom(getMetadata().getType())) { // it's a float
            return new FloatConverter();
        } else if (Double.class.isAssignableFrom(getMetadata().getType())) { // it's a double
            return new DoubleConverter();
        }
        return new NumberConverter(); // default
    }

    @Override
    protected InputType createInputType() {
        return InputType.DEFAULT;
    }

}
