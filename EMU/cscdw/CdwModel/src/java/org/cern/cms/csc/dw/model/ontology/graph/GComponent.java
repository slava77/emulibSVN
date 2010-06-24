package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;

public interface GComponent extends GNode {

    void setName(String name);
    String getName();
    boolean isSetName();

    void setType(GComponentClass type);
    GComponentClass getType();
    boolean isSetType();
    Collection<GComponentClass> getTypes();

    void setDescription(String description);
    String getDescription();

    void addSynonym(GComponentSynonym synonym);
    Collection<GComponentSynonym> getSynonyms();

    GComponentLinks getGComponentLinks();
    Collection<GComponentLink> getLinks();

    Collection<GComponent> getRelatedGComponents(ComponentLinkClassType type);
    Collection<GComponent> getRelatedGComponents(ComponentLinkClassType type, Direction dir);

    boolean hasRelatedGComponents(ComponentLinkClassType type);
    boolean hasRelatedGComponents(ComponentLinkClassType type, Direction dir);

}
