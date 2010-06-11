package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Transaction;

public class GComponentClassImpl extends GNodeImpl implements GComponentClass {

    public GComponentClassImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    public void setType(ComponentClassType type) {
        setProperty(PropertyType.TYPE, type.value(), true);
    }

    public ComponentClassType getType() {
        return ComponentClassType.fromValue((String) getProperty(PropertyType.TYPE));
    }

    public void setDescription(String description) {
        setProperty(PropertyType.DESCRIPTION, description);
    }

    public String getDescription() {
        return (String) getProperty(PropertyType.DESCRIPTION, null);
    }

    public Collection<GComponentClass> getChildren() {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GComponentClassImpl.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING);
    }

    public Collection<GComponentClass> getParents() {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GComponentClassImpl.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.OUTGOING);
    }

    public void addParent(GComponentClass parent) {
        if (!getParents().contains(parent)) {
            setRelationship(parent, GLinkType.CLASS_TO_PARENT);
        }
    }

    public void addComponent(GComponent component) {
        if (component.getType() == null) {
            component.setType(this);
        }
    }

    public Collection<GComponent> getComponents() {
        return getRelatedGNodeCollection(
                GComponent.class,
                GComponentImpl.class,
                GLinkType.COMPONENT_TO_CLASS,
                Direction.INCOMING);
    }

    public boolean isHasComponents() {
        Transaction tx = gservices.beginTx();
        try {
            return node.hasRelationship(GLinkType.COMPONENT_TO_CLASS, Direction.INCOMING);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public Collection<GComponentClass> getChildrenTransient() {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GComponentClassImpl.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING);
    }

}
