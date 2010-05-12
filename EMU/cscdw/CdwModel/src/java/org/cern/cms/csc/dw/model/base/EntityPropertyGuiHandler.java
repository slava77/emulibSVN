/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.util.Date;
import javax.faces.convert.BigIntegerConverter;
import javax.faces.convert.Converter;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
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
public class EntityPropertyGuiHandler {

    public enum InputType {
        DEFAULT,
        TEXT_AREA,
        DATE_PICKER,
        SELECT_ONE_MENU
    }

    /** Tells what kind of input to use in the GUI (e.g. default, textArea, date). */
    private InputType inputType;
    /** JSF converter for this property type. */
    private Converter jsfConverter;
    /** Metadata of the property which this GUI handler is for. */
    private EntityPropertyMD propMetadata;

    /**
     * Constructor.
     * @param propMetadata property metadata object.
     */
    public EntityPropertyGuiHandler(EntityPropertyMD propMetadata) {
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
        String errorMsg = getPropMetadata().validate(value);
        if (errorMsg != null) {
            FacesMessage msg = new FacesMessage(errorMsg);
            ((UIInput)validate).setValid(false);
            context.addMessage(validate.getClientId(context), msg);
        }
    }

    /**
     * Get list of available values as List<SelectItem>
     * @return list of available values as List<SelectItem>
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public List<SelectItem> getListOfValues() throws Exception {
        List<Object> lov = getPropMetadata().getListOfValues();
        if (lov != null) {
            List<SelectItem> ret = new ArrayList<SelectItem>();
            for (Object lovObject: lov) {
                SelectItem lovItem = new SelectItem(lovObject, lovObject.toString());
                ret.add(lovItem);
            }
            return ret;
        }
        return null;
    }

}
