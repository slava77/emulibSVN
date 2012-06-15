/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.validator;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.validator.Validator;
import javax.faces.validator.ValidatorException;

/**
 *
 * @author valdo
 */
public class UrlValidator implements Validator {

    private boolean ping = false;
    
    public void validate(FacesContext context, UIComponent component, Object value) throws ValidatorException {
        String surl = (String) value;
        try {
            URL url = new URL(surl);
            if (ping) {
                HttpURLConnection connection = (HttpURLConnection) url.openConnection();
                connection.setRequestMethod("HEAD");
                int code = connection.getResponseCode();
                if (code != 200) {
                    throw new IOException("HTTP command HEAD did not return 200 (was " + String.valueOf(code) + ")?!");
                }
            }
        } catch (MalformedURLException e) {
            throw new ValidatorException(new FacesMessage("Malformed URL [" + surl + "]."), e);
        } catch (IOException e) {
            throw new ValidatorException(new FacesMessage("Connection to [" + surl + "] couldn't be established."), e);
        }
    }

    public void setPing(boolean ping) {
        this.ping = ping;
    }
    
}
