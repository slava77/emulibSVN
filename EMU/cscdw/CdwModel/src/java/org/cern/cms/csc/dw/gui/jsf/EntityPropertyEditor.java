/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.gui.jsf;

import org.cern.cms.csc.dw.model.base.*;
import java.util.Date;
import javax.faces.convert.BigIntegerConverter;
import javax.faces.convert.Converter;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import javax.faces.model.SelectItem;

/**
 *
 * @author Evka
 */
public class EntityPropertyEditor {

    public enum InputType {
        DEFAULT,
        TEXT_AREA,
        DATE_PICKER,
        SELECT_ONE_MENU
    }

    private static Logger logger = Logger.getLogger(EntityPropertyEditor.class.getName());

    /** Tells what kind of input to use in the GUI (e.g. default, textArea, date). */
    private InputType inputType;
    /** JSF converter for this property type. */
    private Converter jsfConverter;
    /** Metadata of the property which this GUI handler is for. */
    private EntityPropertyMD propMetadata;
    /** Cache of list of available values (used only for enum, many-to-one and many-to-many properties). */
    private List<Object> lovCache = null;
    /** Cache of list of available select items (used only for enum, many-to-one and many-to-many properties). */
    private List<SelectItem> lovSelectItemsCache = null;
    /** If List Of Values is used then this flag tells if it's strict (meaning that no new values can be created) or not (meaning that the returned LOV should include a new value as well). */
    private boolean isLovStrict = true;

    /**
     * Constructor.
     * @param propMetadata property metadata object.
     */
    public EntityPropertyEditor(EntityPropertyMD propMetadata) {
        this.propMetadata = propMetadata;
        Class propType = propMetadata.getType();
        // basic property
        if (propMetadata instanceof EntityBasicPropertyMD) {
            EntityBasicPropertyMD basicPropMetadata = (EntityBasicPropertyMD) propMetadata;
            if (Date.class.isAssignableFrom(propType)) {
                inputType = InputType.DATE_PICKER;
            } else if (propType.equals(String.class) && (basicPropMetadata.getLength() > 512)) {
                inputType = InputType.TEXT_AREA;
            }
        } else if (propMetadata instanceof EntityManyToOnePropertyMD) { // many to one property
            inputType = InputType.SELECT_ONE_MENU;
            isLovStrict = false;
        }
        if (inputType == null) {
            inputType = InputType.DEFAULT;
        }
    }

    /**
     * Get input type, which tells what kind of input to use in the GUI (e.g. default, textArea, date).
     * @return input type, which tells what kind of input to use in the GUI (e.g. default, textArea, date).
     */
    public InputType getInputType() {
        return inputType;
    }

    /**
     * Get metadata of the property which this GUI handler is for.
     * @return metadata of the property which this GUI handler is for.
     */
    private EntityPropertyMD getPropMetadata() {
        return propMetadata;
    }

    /**
     * Get JSF converter for this property type.
     * @return JSF converter for this property type.
     */
    public Converter getJsfConverter() {
        if (jsfConverter == null) {
            // it's a number
            if (Number.class.isAssignableFrom(getPropMetadata().getType())) {
                // it's an integer
                if (BigInteger.class.isAssignableFrom(getPropMetadata().getType())) {
                    jsfConverter = new BigIntegerConverter();
                } else { // default number converter
                    jsfConverter = new NumberConverter();
                }
            } else if (Date.class.isAssignableFrom(getPropMetadata().getType())) { // it's a date
                jsfConverter = new DateTimeConverter();
            } else if (getInputType() == InputType.SELECT_ONE_MENU) {
                if (lovCache == null) {
                    try {
                        getListOfValues();
                    } catch(Exception ex) {
                        throw new RuntimeException(ex);
                    }
                }
                jsfConverter = new LovConverter(lovCache);
            }
        }

        return jsfConverter;
    }

    /**
     * Validation method for JSF. Uses EntityPropertyMD.validate(Object value) to validate and form an error message.
     * @param context JSF context
     * @param validate UIComponent to be validated
     * @param value value to be validated
     */
    public void validate(FacesContext context, UIComponent validate, Object value) {
        logger.info("Got " + value.getClass().getName() + " value to validate");
        logger.info("Value = " + value);
        String errorMsg = getPropMetadata().validate(value);
        if (errorMsg != null) {
            FacesMessage msg = new FacesMessage(errorMsg);
            ((UIInput)validate).setValid(false);
            context.addMessage(validate.getClientId(context), msg);
        }
    }

    /**
     * Get list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @return list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public List<SelectItem> getListOfValues() throws Exception {
        return getListOfValues(!isLovStrict);
    }

    /**
     * Get list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @param includeNewValue if this is set to true, then a new value is also included in the list
     * @return list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public List<SelectItem> getListOfValues(boolean includeNewValue) throws Exception {
        if (lovSelectItemsCache == null) {
            refreshListOfValues(includeNewValue);
        }
        return lovSelectItemsCache;
    }

    /**
     * Get list of available values as List<SelectItem>
     * @return list of available values as List<SelectItem>
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public void refreshListOfValues() throws Exception {
        refreshListOfValues(false);
    }

    /**
     * Get list of available values as List<SelectItem>
     * @param includeNewValue if this is set to true, then a new value is also included in the list
     * @return list of available values as List<SelectItem>
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public void refreshListOfValues(boolean includeNewValue) throws Exception {
        lovCache = getPropMetadata().getListOfValues();
        if (lovCache != null) {
            List<SelectItem> ret = new ArrayList<SelectItem>();
            for (Object lovObject: lovCache) {
                SelectItem lovItem;
                if (lovObject instanceof EntityBase) {
                    EntityBase lovEntityObj = (EntityBase) lovObject;
                    lovItem = new SelectItem(lovObject, lovEntityObj.getEntityTitle());
                } else {
                    lovItem = new SelectItem(lovObject, lovObject.toString());
                }
                ret.add(lovItem);
            }
            if (includeNewValue) {
                lovCache.add(0, getPropMetadata().getNewValue());
                SelectItem createNewSI = new SelectItem(getPropMetadata().getNewValue(), "Create New...");
                ret.add(0, createNewSI);
            }
            lovSelectItemsCache = ret;
        }
    }

}
