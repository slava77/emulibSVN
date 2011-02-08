/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.complex;

import java.util.Collections;
import java.util.List;
import javax.faces.convert.Converter;
import javax.faces.model.SelectItem;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.metadata.PropertyMd;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.component.OntologyComponentSelector;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;
import org.cern.cms.csc.exsys.gui.editor.base.Editor.InputType;

/**
 *
 * @author valdo
 */
public class ComponentEditor extends ManyToOneEditor {

    private OntologyComponentSelector componentSelector;


    public ComponentEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EditorDaoLocal EditorDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, EditorDao);
        this.componentSelector = new OntologyComponentSelector() {
            @Override
            protected GOntologyDaoLocal getGOntologyDao() {
                return getEditorDao().getGOntologyDao();
            }
        };

    }

    @Override
    protected Converter createConverter() {
        return null;
    }

    @Override
    protected InputType createInputType() {
        return InputType.COMPONENT;
    }

    @Override
    public List<SelectItem> getListOfValues() throws Exception {
        return Collections.EMPTY_LIST;
    }

    @Override
    public OntologyComponentSelector getComponentSelector() {
        return componentSelector;
    }

}
