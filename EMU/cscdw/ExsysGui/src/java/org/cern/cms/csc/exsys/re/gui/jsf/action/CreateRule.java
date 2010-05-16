/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import org.cern.cms.csc.exsys.re.model.ConclusionFactory;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */

@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class CreateRule {

    private static Logger logger = Logger.getLogger(CreateRule.class.getName());

    /** Rule to be created. */
    Rule rule;

    /** Creates a new instance of CreateRule */
    public CreateRule() {
        rule = new Rule();
    }

    /**
     * @return rule to be created.
     */
    public Rule getRule() {
        return rule;
    }

    public ConclusionFactory getTest() {
        return new ConclusionFactory();
    }

    /**
     * @return saves the rule to DB.
     */
    public String doSave() {
        return "success";
    }

    public void validateEmail(FacesContext context, UIComponent validate, Object value) {
        FacesMessage msg = null;
        if (value == null) {
            msg = new FacesMessage("This field is mandatory - value cannot be blank");
        }
        if (value.equals("bad")) {
            msg = new FacesMessage("Please don't enter \"bad\" :)");
        }

        if (msg != null) {
            ((UIInput)validate).setValid(false);
            context.addMessage(validate.getClientId(context), msg);
        }
    }
}
