package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;

public interface GComponentLinkClass extends GNode {

    void setType(ComponentLinkClassType type);
    ComponentLinkClassType getType();

    void setDescription(String description);
    String getDescription();

    void setParent(GComponentLinkClass parent);
    GComponentLinkClass getParent();
    Collection<GComponentLinkClass> getChildren();

    void setInverse(GComponentLinkClass inverse);
    GComponentLinkClass getInverse();

    Boolean getTransitive();
    void setTransitive(Boolean transitive);

}
