package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;

public interface GComponentLinks extends GNode {

    void addLink(GComponent component, ComponentLinkClassType type);
    Collection<GComponentLink> getLinks();

    boolean hasLink(GComponent component, ComponentLinkClassType type);

}
