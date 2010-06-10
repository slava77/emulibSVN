/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor;

import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.RestrictedEntityEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.complex.TopEntityEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;

/**
 * This bean is used by the generic entity editor facelet.
 * @author Evka
 */

@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public abstract class EntityEditorManager extends JsfBeanBase {

    /** Top level editor. */
    private TopEntityEditor topEditor;
    /** Editor that's currently displayed. */
    private Editor currentEditor;
    /** Entity DAO. */
    private EntityDaoLocal entityDao;

    /**
     * Constructor
     * @param topEditor - top level editor to be used
     */
    public EntityEditorManager(EntityBase entityToEdit) {
        super();
        try {
            this.entityDao = (EntityDaoLocal) getEjb("ejb/EntityDao");
            this.topEditor = new TopEntityEditor(entityToEdit, entityDao);
            this.currentEditor = topEditor;
        } catch (NamingException nex) {
            throw new RuntimeException("Failed to instantiate EntityEditorManager - exception while looking up EntityDao EJB", nex);
        } catch (InvalidEntityBeanPropertyException iepex) {
            throw new RuntimeException("Failed to instantiate EntityEditorManager - invalid entity bean property exception thrown while creating TopEntityEditor", iepex);
        }
    }

    /**
     * Get top level editor.
     * @return top level editor.
     */
    public TopEntityEditor getTopEditor() {
        return topEditor;
    }

    /**
     * Get the current editor that's actually being used (displayed) right now.
     * @return the current editor that's actually being used (displayed) right now.
     */
    public Editor getEditor() {
        return currentEditor;
    }

    /**
     * Set current editor - that's called from GUI when user goes inside to edit a related entity.
     * @param editor editor to set as current one.
     */
    public void setEditor(Editor editor) {
        this.currentEditor = editor;
        refreshView();
    }

    /**
     * Discards the value of the current editor and goes back (sets the parent editor as the current editor)
     * @return whatever is returned by goBack()
     * @see goBack()
     * @throws Exception
     */
    public String discardAndGoBack() {
        try {
            currentEditor.setValue(null);
            if (currentEditor instanceof RestrictedEntityEditor) {
                ((RestrictedEntityEditor) currentEditor).refreshListOfValues();
            }
        } catch (Exception ex) {
            throw new RuntimeException("Exception while discarding value of an editor. ", ex);
        }
        return goBack();
    }

    /**
     * Goes back - sets the parent editor as the current editor.
     * @return "reload"
     */
    public String goBack() {
        setEditor(currentEditor.getParentEditor());
        return "reload";
    }

    /**
     * Implement this method with a code which will be called when the user is finished editing.
     * @return JSF navigation outcome.
     * @throws Exception - placeholder for whatever exceptions you might want to throw.
     */
    public abstract String doSave() throws Exception;

    /**
     * Get Entity DAO
     * @return Entity DAO
     */
    protected EntityDaoLocal getEntityDao() {
        return entityDao;
    }

}
