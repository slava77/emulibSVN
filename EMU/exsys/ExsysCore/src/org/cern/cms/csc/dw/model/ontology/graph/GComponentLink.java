package org.cern.cms.csc.dw.model.ontology.graph;

import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;

public interface GComponentLink extends GLink {

    GComponent getComponent();
    ComponentLinkClassType getType();

}
