package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;

public interface GComponentClass extends GNode {

    void setType(ComponentClassType type);
    ComponentClassType getType();

    void setDescription(String description);
    String getDescription();

    void addParent(GComponentClass parent);
    Collection<GComponentClass> getParents();
    Collection<GComponentClass> getChildren();
    Collection<GComponentClass> getChildrenTransient();

    Collection<GComponent> getComponents();
    void addComponent(GComponent component);
    boolean isHasComponents();

}
