/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.complex;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import org.cern.cms.csc.dw.dao.GenericDaoLocal;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;
import org.cern.cms.csc.exsys.gui.editor.base.Editor.InputType;
import org.cern.cms.csc.exsys.gui.editor.base.EntityEditor;
import org.cern.cms.csc.exsys.gui.editor.base.RestrictedEntityEditor;
import org.cern.cms.csc.exsys.gui.util.ClassFinder;

/**
 *
 * @author evka
 */
public class EditorClassEditor extends RestrictedEntityEditor {

    private final static Logger logger = SimpleLogger.getLogger(EditorClassEditor.class);

    private Class value;

    public EditorClassEditor(EntityBase entity, Editor parentEditor, GenericDaoLocal genericDao) throws InvalidEntityBeanPropertyException {
        super(entity, null, parentEditor, genericDao);
        if (entity != null) {
            this.value = entity.getClass();
        }
    }

    @Override
    public void setValue(Object value) throws IllegalAccessException, InvocationTargetException {
        if (value == null) {
            return;
        }

        Class clazz = (Class) value;
        if (!clazz.equals(this.value)) {
            this.value = clazz;

            EntityEditor parentEditor = (EntityEditor) getParentEditor();
            try {
                parentEditor.setValue(clazz.newInstance());
            } catch (InstantiationException instEx) {
                throw new RuntimeException("Could not create a new instance of class " + clazz.getCanonicalName());
            }
        }
    }

    @Override
    public Object getValue() throws IllegalAccessException, InvocationTargetException {
        return value;
    }

    /**
     * Get list of available values as List<SelectItem>
     * @return list of available values as List<SelectItem>
     */
    @Override
    public void refreshListOfValues() {
        Class baseClass = getParentEditor().getMetadata().getType();
        try {
            if (lovCache == null) {
                lovCache = new ArrayList<Object>();
            }
            lovCache.clear();
            if (this.value == null) {
                lovCache.add(null);
            }
            List<Class> subclasses = ClassFinder.findSubclassesInSamePackage(baseClass);
            for (Class subclass: subclasses) {
                if (!Modifier.isAbstract(subclass.getModifiers())) {
                    lovCache.add(subclass);
                }
            }
            resetConverter();
            lovSelectItemsCache =  null;
        } catch (Exception ex) {
            throw new RuntimeException("Exception while trying to retrieve subclasses of " + baseClass.getCanonicalName(), ex);
        }
    }

    @Override
    protected String getLovObjectTitle(Object lovObject) throws Exception {
        if (lovObject == null) {
            return "Please choose a class";
        }
        if (!(lovObject instanceof Class)) {
            throw new RuntimeException("EditorClassEditor.getLovObjectTitle got an LOV object which is not a Class");
        }
        Class clazz = (Class) lovObject;
        return clazz.getSimpleName();
    }

    /** Always returns true, telling the GUI not to allow creation of new values. */
    @Override
    public boolean isRestrictionStrict() {
        return true;
    }

    @Override
    protected InputType createInputType() {
        return InputType.SELECT_ONE_MENU;
    }

    @Override
    public void validate(FacesContext context, UIComponent validate, Object value) {
        if (value == null) {
            String errorMsg = "Please choose a classs";
            logger.info("EditorClassEditor validation error: " + errorMsg);
            FacesMessage msg = new FacesMessage(errorMsg);
            ((UIInput)validate).setValid(false);
            context.addMessage(validate.getClientId(context), msg);
        }
    }

}
