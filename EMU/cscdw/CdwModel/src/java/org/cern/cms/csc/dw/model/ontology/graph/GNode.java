package org.cern.cms.csc.dw.model.ontology.graph;

import org.neo4j.graphdb.Node;

public interface GNode {

    public final static String KEY_OBJECT_TYPE = "objType";

    public enum PropertyType {
        NAME,
        CLASS_TYPE,
        DESCRIPTION,
        TRANSITIVE;
    };

    Long getId();
    Node getNode();

}
