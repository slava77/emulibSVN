package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import java.util.Set;
import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;

@Local
public interface GOntologyDaoLocal {

    GComponent getGComponent(Long id) throws ComponentNotFoundException;
    GComponent getGComponent(String name) throws ComponentNotFoundException;
    GComponent getGComponent(Component component) throws ComponentNotFoundException;
    GComponent getGComponentSilent(String name);
    Collection<GComponent> getGComponentsByNameMatches(String query, final Collection<GComponentClass> types, long numResults);

    GComponentClass getGComponentClass(Long id) throws ComponentClassNotFoundException;
    GComponentClass getGComponentClass(ComponentClassType type);
    Collection<GComponentClass> getGComponentClasses();

    GComponentLinkClass getGComponentLinkClass(Long id) throws ComponentLinkClassNotFoundException;
    Collection<GComponentLinkClass> getGComponentLinkClasses();

    boolean isGComponentClassParent(Set<ComponentClassType> parents, GComponentClass toCheck, boolean recursive);

}
