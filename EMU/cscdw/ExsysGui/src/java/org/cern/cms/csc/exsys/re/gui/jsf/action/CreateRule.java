/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.faces.event.ActionEvent;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.util.JsfBeanBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.PropertyEditorFactory;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.EntityEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.complex.ManyToOneEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.complex.TopEntityEditor;
import org.cern.cms.csc.exsys.re.model.ConclusionFactory;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */

@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class CreateRule extends JsfBeanBase {

    private static Logger logger = Logger.getLogger(CreateRule.class.getName());
    
    private EntityDaoLocal entityDao;

    /** Rule editor */
    private TopEntityEditor topEditor;
    /** Editor that's currently displayed. */
    private Editor currentEditor;

    /** Creates a new instance of CreateRule */
    public CreateRule() throws NamingException {
        Rule rule = new Rule();
        try {
            topEditor = new TopEntityEditor(rule);
            currentEditor = topEditor;
            entityDao = (EntityDaoLocal) getEjb("ejb/EntityDao");
        } catch (InvalidEntityBeanPropertyException ex) {
            throw new RuntimeException(ex);
        }
    }

    /**
     * @return rule to be created.
     */
    public Editor getEditor() {
        if (currentEditor.getMetadata() != null) {
            logger.info("getEditor() = " + currentEditor.getMetadata().getName());
        } else {
            logger.info("getEditor() = TOP");
        }
        return currentEditor;
    }

    public void setEditor(Editor editor) {
        try {
        logger.info("!!!!!!!! Setting editor with value: " + editor.getValue());
        }catch(Exception ex) {}
        this.currentEditor = editor;
    }

    public String testGoInside() {
        List<Editor> propEditors = topEditor.getPropertyEditors();
        for (Editor propEditor: propEditors) {
            if (propEditor instanceof ManyToOneEditor) {
                currentEditor = propEditor;
            }
        }
        logger.info("Current editor changed to " + currentEditor.getMetadata().getName());
        return "reload";
    }

    public String goUp() {
        currentEditor = currentEditor.getParentEditor();
        if (currentEditor.getMetadata() != null) {
            logger.info("Current editor changed to " + currentEditor.getMetadata().getName());
        } else {
            logger.info("Current editor changed to TOP");
        }
        return "reload";
    }

    public ConclusionFactory getTest() {
        return new ConclusionFactory();
    }

    /**
     * @return saves the rule to DB.
     */
    public String doSave() throws PersistException, OnSaveProcessingException {
        entityDao.getPersistDao().persist(topEditor.getValue());
        return "navRules";
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
