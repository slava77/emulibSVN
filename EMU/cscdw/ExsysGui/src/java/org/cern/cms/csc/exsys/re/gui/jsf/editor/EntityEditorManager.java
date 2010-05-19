/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor;

import org.cern.cms.csc.exsys.re.gui.jsf.editor.complex.EntityEditor;

/**
 * This bean is used by the generic entity editor facelet.
 * @author Evka
 */

public class EntityEditorManager {

    /** Top level editor. */
    private EntityEditor topEditor;

    /**
     * Constructor
     * @param topEditor - top level editor to be used
     */
    public EntityEditorManager(EntityEditor topEditor) {
        this.topEditor = topEditor;
    }

    /**
     * Get top level editor.
     * @return top level editor.
     */
    public EntityEditor getTopEditor() {
        return topEditor;
    }

    /**
     * Get the current editor that's actually being used (displayed) right now.
     * @return the current editor that's actually being used (displayed) right now.
     */
    public EntityEditor getCurrentEditor() {
        return topEditor;
    }

}
