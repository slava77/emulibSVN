package org.cern.cms.csc.dw.model.ontology.graph;

import org.neo4j.graphdb.Relationship;

public interface GLink {

    Long getId();
    Relationship getRelationship();

}
