package org.cern.cms.csc.dw.ws;

import java.util.Set;
import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;

@Local
public interface ComponentPropertyResolverLocal {

    boolean checkComponentType(Component component, Set<ComponentClassType> checkParents, boolean checkRecursive)
            throws ComponentNotFoundException;

    boolean checkComponentType(GComponent gcomponent, Set<ComponentClassType> checkParents, boolean checkRecursive);

    Object resolveComponentLinks(Object bean) throws Exception;

}
