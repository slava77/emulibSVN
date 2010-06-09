package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;

public interface GComponent extends GNode {

    void setName(String name);
    String getName();

    void setType(GComponentClass type);
    GComponentClass getType();
    boolean isSetType();

    void setDescription(String description);
    String getDescription();

    void addSynonym(GComponentSynonym synonym);
    Collection<GComponentSynonym> getSynonyms();

    GComponentLinks getGComponentLinks();
    Collection<GComponentLink> getLinks();

}
