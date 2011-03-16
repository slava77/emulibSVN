package org.cern.cms.csc.dw.model.ontology.graph;

import org.neo4j.graphdb.Node;

public interface GNode extends Comparable {

    public final static String KEY_OBJECT_TYPE = "objType";

    public interface PropertyType {
        String propertyName();
    }

    public enum InternalPropertyType implements PropertyType {
        NAME,
        TYPE,
        DESCRIPTION,
        TRANSITIVE;

        @Override
        public String propertyName() {
            return "Internal." + name();
        }
    };

    Long getId();
    Node getNode();

}
