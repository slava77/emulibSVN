/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor;

import java.util.ArrayList;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import java.util.Date;
import java.util.List;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.base.metadata.EntityBasicPropertyMD;
import org.cern.cms.csc.dw.model.base.metadata.EntityManyToOnePropertyMD;
import org.cern.cms.csc.dw.model.base.metadata.EntityOneToOnePropertyMD;
import org.cern.cms.csc.dw.model.base.metadata.EntityPropertyMD;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.basic.DatePropertyEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.basic.NumberPropertyEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.basic.StringPropertyEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.complex.ManyToOneEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.complex.OneToOneEditor;

/**
 * This is a factory for property editors - analyzes the property metadata and creates appropriate subclass of Editor.
 * @author Evka
 */
public class PropertyEditorFactory {

    /**
     * Create property editors for all properties of the given entity
     * @param entity entity whose properties are to be edited
     * @param parentEditor editor the returned editors should belong to.
     * @return a list of property editors for all properties of the given entity
     */
    public static List<Editor> createPropertyEditors(EntityBase entity, Editor parentEditor) {
        try {
            List<Editor> ret = new ArrayList<Editor>();
            List<EntityPropertyMD> propsMetadata = entity.getPropertyMetadata();
            for (EntityPropertyMD propMetadata: propsMetadata) {
                ret.add(createPropertyEditor(entity, propMetadata, parentEditor));
            }
            return ret;
        } catch (InvalidEntityBeanPropertyException ex) {
            throw new RuntimeException(ex);
        }
    }

    /**
     * Create a property editor
     * @param entity entity whose property is to be edited
     * @param propMetadata metadata of the property to be edited
     * @param parentEditor editor the returned editor should belong to.
     * @return property editor for the given property
     */
    public static Editor createPropertyEditor(EntityBase entity, EntityPropertyMD propMetadata, Editor parentEditor) {
        try {
            Class propType = propMetadata.getType();
            // basic property
            if (propMetadata instanceof EntityBasicPropertyMD) {
                if (Date.class.isAssignableFrom(propType)) {
                    return new DatePropertyEditor(entity, propMetadata, parentEditor);
                } else if (Number.class.isAssignableFrom(propType)) {
                    return new NumberPropertyEditor(entity, propMetadata, parentEditor);
                } else if (String.class.isAssignableFrom(propType)) {
                    return new StringPropertyEditor(entity, propMetadata, parentEditor);
                }
            } else if (propMetadata instanceof EntityManyToOnePropertyMD) { // many to one property
                return new ManyToOneEditor(entity, propMetadata, parentEditor);
            } else if (propMetadata instanceof EntityOneToOnePropertyMD) { // one to one property
                return new OneToOneEditor(entity, propMetadata, parentEditor);
            }
            return null;
        } catch (InvalidEntityBeanPropertyException ex) {
            throw new RuntimeException(ex);
        }
    }

}
