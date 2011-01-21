/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor;

import org.cern.cms.csc.dw.dao.GenericDaoLocal;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;
import org.cern.cms.csc.exsys.gui.editor.base.RestrictedEntityEditor;
import org.cern.cms.csc.exsys.gui.editor.complex.TopEntityEditor;
import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;

/**
 * This bean is used by the generic entity editor facelet.
 * @author Evka
 */

public abstract class EntityEditorManager extends JsfBeanBase {

    private static Logger logger = SimpleLogger.getLogger(EntityEditorManager.class);

    /** Top level editor. */
    private TopEntityEditor topEditor;
    /** Editor that's currently displayed. */
    private Editor currentEditor;
    /** Entity that is being edited by this editor manager. */
    private EntityBase entity;

    protected abstract GenericDaoLocal getGenericDao();

    /**
     * Constructor
     * @param entityToEdit - entity to be edited by this editor manager
     */
    public EntityEditorManager(EntityBase entityToEdit) {
        this();
        this.entity = entityToEdit;
    }

    /**
     * Constructor
     */
    public EntityEditorManager() {
        super();
    }

    /**
     * Get entity that is being edited by this editor manager.
     * @return entity that is being edited by this editor manager.
     */
    public EntityBase getEntity() {
        return entity;
    }

    /**
     * Set entity that is being edited by this editor manager.
     * @param entity entity that is being edited by this editor manager.
     */
    public void setEntity(EntityBase entity) {
        if (((entity != null) && !entity.equals(this.entity)) ||
            ((this.entity != null) && !this.entity.equals(entity))) {
            reset();
        }
        this.entity = entity;
    }

    /** Resets the manager. */
    public void reset() {
        this.entity = null;
        this.topEditor = null;
        this.currentEditor = null;
    }

    /**
     * Creates a new instance of the entity to be edited. 
     * You must override this method and return a new instance of the entity class that you want your EditorManager to be editing.
     * @return a new instance of the entity to be edited
     */
    protected abstract EntityBase createEntity() throws Exception;

    /**
     * Get top level editor.
     * @return top level editor.
     */
    public TopEntityEditor getTopEditor() {
        if (topEditor == null) {
            try {
                if (entity == null) {
                    entity = createEntity();
                }
                if (entity == null) {
                    return null;
                }
                topEditor = new TopEntityEditor(entity, getGenericDao());
                currentEditor = topEditor;
            } catch (InvalidEntityBeanPropertyException iepex) {
                throw new RuntimeException("Failed to instantiate EntityEditorManager topEditor - invalid entity bean property exception thrown while creating TopEntityEditor", iepex);
            } catch (Exception ex) {
                throw new RuntimeException("Exception while creating a new entity in EntityEditorManager.createEntity()", ex);
            }
        }
        return topEditor;
    }

    /**
     * Get the current editor that's actually being used (displayed) right now.
     * @return the current editor that's actually being used (displayed) right now.
     */
    public Editor getEditor() {
        if (currentEditor == null) {
            getTopEditor(); // this will trigger creation of a new top editor if it doesn't exist and currentEditor will be assigned with that one as well
        }
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

}
