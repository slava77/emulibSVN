package org.cern.cms.csc.dw.model.ontology.graph;

import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;

public class GComponentSynonymImpl extends GNodeImpl implements GComponentSynonym {

    public GComponentSynonymImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    public void setName(String name) {
        setProperty(PropertyType.NAME, name, true);
    }

    public String getName() {
        return (String) getProperty(PropertyType.NAME);
    }

    public GComponent getComponent() {
        return getRelatedGNode(
                GComponent.class,
                GLinkType.COMPONENT_TO_SYNONYM,
                Direction.INCOMING);
    }

    public void setComponent(GComponent component) {
        component.addSynonym(this);
    }

}
