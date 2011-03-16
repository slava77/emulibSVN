package org.cern.cms.csc.dw.model.ontology.graph;

import org.neo4j.graphdb.RelationshipType;

public enum GLinkType implements RelationshipType {

    // Links to reference node
    TYPE_COMPONENT_CLASS,
    TYPE_COMPONENT_LINK_CLASS,
    TYPE_COMPONENT,
    TYPE_COMPONENT_SYNONYM,
    
    // General links
    CLASS_TO_PARENT,
    LINK_TO_PARENT,
    LINK_TO_INVERSE,
    COMPONENT_TO_CLASS,
    COMPONENT_TO_SYNONYM,
    COMPONENT_TO_LINKS

}
