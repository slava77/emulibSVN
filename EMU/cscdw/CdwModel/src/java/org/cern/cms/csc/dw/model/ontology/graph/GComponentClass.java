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
    Collection<GComponentClass> getParentsRecursive();
    Collection<GComponentClass> getParentsRecursive(boolean returnSelf);
    Collection<GComponentClass> getChildren();
    Collection<GComponentClass> getChildrenRecursive();
    Collection<GComponentClass> getChildrenRecursive(boolean returnSelf);

    Collection<GComponent> getComponents();
    Collection<GComponent> getComponentsRecursive();
    void addComponent(GComponent component);
    boolean isHasComponents();
    boolean isHasComponentsRecursive();

}
