/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.base;

import java.lang.reflect.InvocationTargetException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.faces.convert.Converter;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * Objects of this class are used in GUI to setup an input for a specific entity property and actually set and get the value of that property.
 * @author Evka
 */
public abstract class Editor {

    protected static Logger logger = Logger.getLogger(Editor.class.getName());

    public enum InputType {
        DEFAULT,
        TEXT_AREA,
        DATE_PICKER,
        CHECKBOX,
        SELECT_ONE_MENU,
        SELECT_MANY_LIST,
        ONE_TO_ONE,
        COMPONENT
    }

    /** Parent editor that this editor belongs to. */
    private Editor parentEditor;
    /** Reference to the entity instance. */
    private EntityBase entity;
    /** Property metadata. */
    private PropertyMd metadata;
    /** Tells what kind of input to use in the GUI (e.g. default, textArea, date). */
    private InputType inputType = null;
    /** JSF converter for this property type. */
    private Converter converter = null;

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public Editor(EntityBase entity, PropertyMd metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        if ((entity == null) && (metadata != null)) { // only when metadata is not null, because there's sort of a special case with EditorClassEditor which passes in both entity and metadata as null, because it doesn't have them (nor does it need them)
            throw new NullPointerException("Trying to create a property editor for a null entity");
        }
        this.entity = entity;
        this.metadata = metadata;
        this.parentEditor = parentEditor;
    }

    public int getId() {
        return this.toString().hashCode();
    }

    /**
     * Get the property value.
     * @return the property value.
     */
    public Object getValue() throws IllegalAccessException, InvocationTargetException {
        return getMetadata().getGetterMethod().invoke(getEntity());
    }

    /**
     * Set the property value.
     * @param value the property value.
     */
    public void setValue(Object value) throws IllegalAccessException, InvocationTargetException {
        getMetadata().getSetterMethod().invoke(getEntity(), value);
    }

    /**
     * Get reference to the entity instance.
     * @return reference to the entity instance.
     */
    public EntityBase getEntity() {
        return entity;
    }

    /**
     * Get property metadata.
     * @return property metadata.
     */
    public PropertyMd getMetadata() {
        return metadata;
    }

    /**
     * Get parent editor that this editor belongs to.
     * @return parent editor that this editor belongs to.
     */
    public Editor getParentEditor() {
        return parentEditor;
    }

    /**
     * Get JSF converter for this property type.
     * @return JSF converter for this property type.
     */
    public Converter getConverter() {
        if (converter == null) {
            converter = createConverter();
        }
        return converter;
    }

    protected void resetConverter() {
        converter = null;
    }

    /**
     * Get what kind of input to use in the GUI (e.g. default, textArea, date).
     * @return kind of input to use in the GUI (e.g. default, textArea, date).
     */
    public InputType getInputType() {
        if (inputType == null) {
            inputType = createInputType();
        }
        return inputType;
    }

    /**
     * Validation method for JSF. Uses PropertyMd.validate(Object value) to validate and form an error message.
     * @param context JSF context
     * @param validate UIComponent to be validated
     * @param value value to be validated
     */
    public void validate(FacesContext context, UIComponent validate, Object value) {
        String errorMsg = getMetadata().validate(value);
        if (errorMsg != null) {
            logger.info("!!!! Editor: validation error: " + errorMsg);
            FacesMessage msg = new FacesMessage(errorMsg);
            ((UIInput)validate).setValid(false);
            context.addMessage(validate.getClientId(context), msg);
        }
    }

    /**
     * Create inputType flag. This method is used by get getInputType() for lazy initialization of the inputType.
     * @return inputType flag.
     */
    protected abstract InputType createInputType();

    /**
     * Creates a converter. This method is used by get getConverter() for lazy initialization of converter.
     * It is responsibility of concrete Editor subclasses to return the correct converter for the type they're dealing with.
     * @return JSF converter for this property type.
     */
    protected abstract Converter createConverter();

}
