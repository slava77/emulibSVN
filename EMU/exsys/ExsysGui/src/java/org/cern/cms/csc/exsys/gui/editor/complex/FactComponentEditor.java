/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.editor.complex;

import java.util.Collection;
import java.util.TreeSet;
import org.cern.cms.csc.dw.dao.GenericDaoLocal;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.gui.editor.base.Editor;

/**
 *
 * @author valdo
 */
public class FactComponentEditor extends ComponentEditor {

    private Collection<GComponentClass> limitComponentClasses;

    public FactComponentEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, GenericDaoLocal genericDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, genericDao);
        limitComponentClasses = new TreeSet<GComponentClass>();
        for (ComponentClassType ct: ((Fact) getEntity()).getMetadata().getLimitComponents()) {
            limitComponentClasses.addAll(genericDao.getGOntologyDao().getGComponentClass(ct).getChildrenRecursive(true));
        }
    }

    public Collection<GComponentClass> getComponentClassLimit() {
        return limitComponentClasses;
    }

}
