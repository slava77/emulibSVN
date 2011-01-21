/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor;

import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import org.cern.cms.csc.dw.dao.GenericDaoLocal;
import org.cern.cms.csc.dw.dao.GenericDaoLocal;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.BasicPropertyMd;
import org.cern.cms.csc.dw.metadata.EnumPropertyMd;
import org.cern.cms.csc.dw.metadata.ManyToManyPropertyMd;
import org.cern.cms.csc.dw.metadata.ManyToOnePropertyMd;
import org.cern.cms.csc.dw.metadata.OneToOnePropertyMd;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;
import org.cern.cms.csc.exsys.gui.editor.basic.BooleanPropertyEditor;
import org.cern.cms.csc.exsys.gui.editor.basic.DatePropertyEditor;
import org.cern.cms.csc.exsys.gui.editor.basic.DoublePropertyEditor;
import org.cern.cms.csc.exsys.gui.editor.basic.NumberPropertyEditor;
import org.cern.cms.csc.exsys.gui.editor.basic.StringPropertyEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.ComponentEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.EditorClassEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.EnumValueEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.FactComponentEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.ManyToManyEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.ManyToOneEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.OneToOneEditor;

/**
 * This is a factory for property editors - analyzes the property metadata and creates appropriate subclass of Editor.
 * @author Evka
 */
public class PropertyEditorFactory {

    private static Logger logger = SimpleLogger.getLogger(PropertyEditorFactory.class);

    /**
     * Create property editors for all properties of the given entity
     * @param entity entity whose properties are to be edited
     * @param parentEditor editor the returned editors should belong to.
     * @return a list of property editors for all properties of the given entity
     */
    public static List<Editor> createPropertyEditors(EntityBase entity, Editor parentEditor, GenericDaoLocal genericDao) {
        try {
            List<Editor> ret = new ArrayList<Editor>();

            // if the class of the parent editor is abstract, then plug in the EditorClassEditor
            if ((parentEditor.getMetadata() != null) && Modifier.isAbstract(parentEditor.getMetadata().getType().getModifiers())) {
                EditorClassEditor classEditor = new EditorClassEditor(entity, parentEditor, genericDao);
                ret.add(classEditor);
            }
            
            if (entity == null) {
                return ret;
            }

            List<PropertyMd> propsMetadata = entity.getPropertyMetadata();
            for (PropertyMd propMetadata: propsMetadata) {
                if (propMetadata.getIsManualInputAllowed()) {
                    ret.add(createPropertyEditor(entity, propMetadata, parentEditor, genericDao));
                } else if (propMetadata.getIsCreateDefaultValue()) {
                    try {
                        Object defValue = propMetadata.getType().newInstance();
                        propMetadata.getSetterMethod().invoke(entity, defValue);
                    } catch (Exception ex) {
                        throw new InvalidEntityBeanPropertyException("Could not create default value for " +
                                                                      propMetadata.getEntityClass().getName() +
                                                                      " property " +
                                                                      propMetadata.getName());
                    }
                }
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
    public static Editor createPropertyEditor(EntityBase entity, PropertyMd propMetadata, Editor parentEditor, GenericDaoLocal genericDao) {
        try {
            Class propType = propMetadata.getType();
            // basic property
            if (propMetadata instanceof BasicPropertyMd) {
                if (Date.class.isAssignableFrom(propType)) {
                    return new DatePropertyEditor(entity, propMetadata, parentEditor);
                } else if (Boolean.class.isAssignableFrom(propType) || boolean.class.isAssignableFrom(propType)) {
                    return new BooleanPropertyEditor(entity, propMetadata, parentEditor);
                } else if (Number.class.isAssignableFrom(propType)) {
                    return new NumberPropertyEditor(entity, propMetadata, parentEditor);
                } else if (double.class.isAssignableFrom(propType)) {
                    return new DoublePropertyEditor(entity, propMetadata, parentEditor);
                } else if (String.class.isAssignableFrom(propType)) {
                    return new StringPropertyEditor(entity, propMetadata, parentEditor);
                }
            } else if (propMetadata instanceof ManyToOnePropertyMd) { // many to one property
                if (propMetadata.getType().equals(Component.class)) {
                    if (entity instanceof Fact) {
                        return new FactComponentEditor(entity, propMetadata, parentEditor, genericDao);
                        //return new ComponentEditor(entity, propMetadata, parentEditor, genericDao);
                    } else {
                        return new ComponentEditor(entity, propMetadata, parentEditor, genericDao);
                    }
                }
                return new ManyToOneEditor(entity, propMetadata, parentEditor, genericDao);
            } else if (propMetadata instanceof OneToOnePropertyMd) { // one to one property
                return new OneToOneEditor(entity, propMetadata, parentEditor, genericDao);
            } else if (propMetadata instanceof EnumPropertyMd) { // enum property
                return new EnumValueEditor(entity, propMetadata, parentEditor, genericDao);
            } else if (propMetadata instanceof ManyToManyPropertyMd) { // many to many property
                return new ManyToManyEditor(entity, propMetadata, parentEditor, genericDao);
            }
            return null;
        } catch (InvalidEntityBeanPropertyException ex) {
            throw new RuntimeException(ex);
        }
    }

}
