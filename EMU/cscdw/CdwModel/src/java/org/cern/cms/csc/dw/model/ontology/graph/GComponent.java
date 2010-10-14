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

    Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType);
    Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType, Direction dir);

    boolean hasRelatedGComponents(ComponentLinkClassType linkType);
    boolean hasRelatedGComponents(ComponentLinkClassType linkType, Direction dir);

    /**
     * Find all particularly related components recursively.
     * @param linkType Link type to look for.
     * @return
     */
    Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType);

    /**
     * Find all related components of particular type.
     * @param linkType Link type to look for.
     * @param type Result type
     * @return
     */
    Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType, GComponentClass type);

}
