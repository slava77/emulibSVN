/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import java.lang.reflect.Modifier;
import java.util.logging.Level;
import javax.faces.convert.Converter;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.OneToOnePropertyMd;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.EntityEditor;

/**
 *
 * @author Evka
 */
public class OneToOneEditor extends EntityEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public OneToOneEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, entityDao);
        if (!(metadata instanceof OneToOnePropertyMd)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a OneToOneEditor for a property which is not a one-to-one relation: " + metadata.getName());
        }

        // create a new value
        try {
            if (!Modifier.isAbstract(metadata.getType().getModifiers())) {
                setValue(getMetadata().getNewValue());
            }
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception while creating a new value for OneToOneEditor for " + metadata.getName() + " property", ex);
            throw new RuntimeException("Exception while creating a new value for OneToOneEditor for " + metadata.getName() + " property", ex);
        }
    }

    @Override
    protected InputType createInputType() {
        return InputType.ONE_TO_ONE;
    }

    /**
     * Returns null - converter is not applicable to this type of editor.
     * @return null - converter is not applicable to this type of editor.
     */
    @Override
    protected Converter createConverter() {
        return null;
    }

}
