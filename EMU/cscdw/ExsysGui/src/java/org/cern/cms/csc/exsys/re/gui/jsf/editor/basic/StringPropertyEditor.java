/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.basic;

import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.*;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.base.EntityPropertyMD;

/**
 *
 * @author Evka
 */
public class StringPropertyEditor extends Editor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     */
    public StringPropertyEditor(EntityBase entity, EntityPropertyMD metadata) {
        super(entity, metadata);
    }



}
