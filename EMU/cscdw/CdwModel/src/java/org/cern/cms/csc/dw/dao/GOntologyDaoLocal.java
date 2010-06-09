package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import java.util.Set;
import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;

@Local
public interface GOntologyDaoLocal {

    GComponent getGComponentById(Long id) throws ComponentNotFoundException;
    GComponent getGComponentByName(String name) throws ComponentNotFoundException;
    GComponent getGComponentByNameSilent(String name);
    Collection<GComponent> getGComponentsByNameMatches(String query, long numResults);

    GComponentClass getGComponentClassById(Long id) throws ComponentClassNotFoundException;
    Collection<GComponentClass> getGComponentClasses();

    GComponentLinkClass getGComponentLinkClassById(Long id) throws ComponentLinkClassNotFoundException;
    Collection<GComponentLinkClass> getGComponentLinkClasses();

    boolean isGComponentClassParent(Set<ComponentClassType> parents, GComponentClass toCheck, boolean recursive);

}
