/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.metadata.EnumPropertyMd;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;

/**
 *
 * @author Evka
 */
public class EnumValueEditor extends ManyToOneEditor {

    public EnumValueEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, entityDao);
        if (!(metadata instanceof EnumPropertyMd)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a EnumValueEditor for a property which is not an enum: " + metadata.getName());
        }
    }
    
    @Override
    protected InputType createInputType() {
        return InputType.SELECT_ONE_MENU;
    }

    /** Always returns true, telling the GUI not to allow creation of new values. */
    @Override
    public boolean isRestrictionStrict() {
        return true;
    }

}
