/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.complex;

import java.lang.reflect.Modifier;
import javax.faces.convert.Converter;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.metadata.OneToOnePropertyMd;
import jsf.bean.gui.metadata.PropertyMd;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;
import org.cern.cms.csc.exsys.gui.editor.base.EntityEditor;

/**
 *
 * @author Evka
 */
public class OneToOneEditor extends EntityEditor {

    private static final Logger logger = ExsysLogger.getLogger(OneToOneEditor.class);

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public OneToOneEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EditorDaoLocal EditorDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, EditorDao);
        if (!(metadata instanceof OneToOnePropertyMd)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a OneToOneEditor for a property which is not a one-to-one relation: " + metadata.getName());
        }

        // create a new value
        try {
            if (!Modifier.isAbstract(metadata.getType().getModifiers())) {
                setValue(getMetadata().getNewValue());
            }
        } catch (Exception ex) {
            logger.error("Exception while creating a new value for OneToOneEditor for " + metadata.getName() + " property", ex);
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
