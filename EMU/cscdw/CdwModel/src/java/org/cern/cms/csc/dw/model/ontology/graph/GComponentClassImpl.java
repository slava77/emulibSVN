package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
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
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING);
    }

    public Collection<GComponentClass> getParents() {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.OUTGOING);
    }

    public Collection<GComponentClass> getParentsRecursive() {
        return getParentsRecursive(false);
    }

    public Collection<GComponentClass> getParentsRecursive(boolean returnSelf) {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.OUTGOING,
                StopEvaluator.END_OF_GRAPH,
                (returnSelf ? ReturnableEvaluator.ALL : ReturnableEvaluator.ALL_BUT_START_NODE));
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

    public Collection<GComponentClass> getChildrenRecursive() {
        return getChildrenRecursive(false);
    }

    public Collection<GComponentClass> getChildrenRecursive(boolean returnSelf) {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING,
                StopEvaluator.END_OF_GRAPH,
                (returnSelf ? ReturnableEvaluator.ALL : ReturnableEvaluator.ALL_BUT_START_NODE));
    }

    public Collection<GComponent> getComponentsRecursive() {
        return getRelatedGNodeCollection(
                GComponent.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING,
                GLinkType.COMPONENT_TO_CLASS,
                Direction.INCOMING,
                StopEvaluator.END_OF_GRAPH,
                ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    public boolean isHasComponentsRecursive() {
        for (GComponentClass cc: getChildrenRecursive(true)) {
            if (cc.isHasComponents()) {
                return true;
            }
        }
        return false;
    }

}
