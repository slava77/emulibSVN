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
import org.cern.cms.csc.dw.model.base.EntityBasicPropertyMD;
import org.cern.cms.csc.dw.model.base.EntityManyToOnePropertyMD;
import org.cern.cms.csc.dw.model.base.EntityPropertyMD;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.basic.DatePropertyEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.basic.NumberPropertyEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.basic.StringPropertyEditor;

/**
 * This is a factory for property editors - analyzes the property metadata and creates appropriate subclass of Editor.
 * @author Evka
 */
public class PropertyEditorFactory {

    /**
     * Create property editors for all properties of the given entity
     * @param entity entity whose properties are to be edited
     * @return a list of property editors for all properties of the given entity
     */
    public static List<Editor> createPropertyEditors(EntityBase entity) throws InvalidEntityBeanPropertyException {
        List<Editor> ret = new ArrayList<Editor>();
        List<EntityPropertyMD> propsMetadata = entity.getPropertyMetadata();
        for (EntityPropertyMD propMetadata: propsMetadata) {
            ret.add(createPropertyEditor(entity, propMetadata));
        }
        return ret;
    }

    /**
     * Create a property editor
     * @param entity entity whose property is to be edited
     * @param propMetadata metadata of the property to be edited
     * @return property editor for the given property
     */
    public static Editor createPropertyEditor(EntityBase entity, EntityPropertyMD propMetadata) {
        Class propType = propMetadata.getType();
        // basic property
        if (propMetadata instanceof EntityBasicPropertyMD) {
            if (Date.class.isAssignableFrom(propType)) {
                return new DatePropertyEditor(entity, propMetadata);
            } else if (Number.class.isAssignableFrom(propType)) {
                return new NumberPropertyEditor(entity, propMetadata);
            } else if (String.class.isAssignableFrom(propType)) {
                return new StringPropertyEditor(entity, propMetadata);
            }
        } else if (propMetadata instanceof EntityManyToOnePropertyMD) { // many to one property
            //SELECT_ONE_MENU;
            //isLovStrict = false;
        }
        return null;
    }

}
