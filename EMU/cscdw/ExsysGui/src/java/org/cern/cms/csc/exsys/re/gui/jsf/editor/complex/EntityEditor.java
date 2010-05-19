/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.*;
import java.util.List;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 * Objects of this class represent an editor of an entity (subclass of EntityBase) and hold property editors for that entity object.
 * @author Evka
 */
public class EntityEditor {

    /** Enity that's being edited by this editor. */
    private EntityBase entity;
    /** Editors of the properties of the entity being edited by this EntityEditor. */
    private List<Editor> propertyEditors;

    /**
     * Constructor
     * @param entity entity to be edited by this editor.
     */
    public EntityEditor(EntityBase entity) {
        this.entity = entity;
    }

    /**
     * Get enity that's being edited by this editor.
     * @return enity that's being edited by this editor.
     */
    public EntityBase getEntity() {
        return entity;
    }

    /**
     * Get editors of the properties of the entity being edited by this EntityEditor.
     * @return editors of the properties of the entity being edited by this EntityEditor.
     */
    public List<Editor> getPropertyEditors() {
        return propertyEditors;
    }

}
