/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.validator;

import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.validator.Validator;
import javax.faces.validator.ValidatorException;
import org.apache.jasper.xmlparser.XMLChar;

/**
 *
 * @author valdo
 */
public class CNameValidator implements Validator {

    public void validate(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        String sname = (String) value;
        if (!XMLChar.isValidName(sname)) {
            throw new ValidatorException(new FacesMessage("Malformed name [" + sname + "]."));
        }
    }

}
