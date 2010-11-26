/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import java.lang.reflect.InvocationTargetException;
import javax.faces.convert.Converter;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor.InputType;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.EntityEditor;

/**
 * This type of editor is used as an entry point to edit an entity and has no parent editors.
 * @author Evka
 */
public class TopEntityEditor extends EntityEditor {

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public TopEntityEditor(EntityBase entity, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, null, null, entityDao);
    }

    /**
     * This constructor is hidden because parameter metadata doesn't apply to objects of this class.
     * Use TopEntityEditor(EntityBase entity) instead.
     */
    private TopEntityEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, null, null, entityDao);
    }

    @Override
    public EntityBase getEntity() {
        return null;
    }

    @Override
    public EntityBase getValue() {
        return super.getEntity();
    }

    @Override
    protected Converter createConverter() {
        return null;
    }

    @Override
    protected InputType createInputType() {
        return null;
    }

}
