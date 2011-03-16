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

    @Override
    public void setType(ComponentClassType type) {
        setProperty(InternalPropertyType.TYPE, type.value(), true);
    }

    @Override
    public ComponentClassType getType() {
        return ComponentClassType.fromValue((String) getProperty(InternalPropertyType.TYPE));
    }

    @Override
    public void setDescription(String description) {
        setProperty(InternalPropertyType.DESCRIPTION, description);
    }

    @Override
    public String getDescription() {
        return (String) getProperty(InternalPropertyType.DESCRIPTION, null);
    }

    @Override
    public Collection<GComponentClass> getChildren() {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING);
    }

    @Override
    public Collection<GComponentClass> getParents() {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.OUTGOING);
    }

    @Override
    public Collection<GComponentClass> getParentsRecursive() {
        return getParentsRecursive(false);
    }

    @Override
    public Collection<GComponentClass> getParentsRecursive(boolean returnSelf) {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.OUTGOING,
                StopEvaluator.END_OF_GRAPH,
                (returnSelf ? ReturnableEvaluator.ALL : ReturnableEvaluator.ALL_BUT_START_NODE));
    }

    @Override
    public void addParent(GComponentClass parent) {
        if (!getParents().contains(parent)) {
            setRelationship(parent, GLinkType.CLASS_TO_PARENT);
        }
    }

    @Override
    public void addComponent(GComponent component) {
        if (component.getType() == null) {
            component.setType(this);
        }
    }

    @Override
    public Collection<GComponent> getComponents() {
        return getRelatedGNodeCollection(
                GComponent.class,
                GLinkType.COMPONENT_TO_CLASS,
                Direction.INCOMING);
    }

    @Override
    public boolean isHasComponents() {
        Transaction tx = gservices.beginTx();
        try {
            return node.hasRelationship(GLinkType.COMPONENT_TO_CLASS, Direction.INCOMING);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    @Override
    public Collection<GComponentClass> getChildrenRecursive() {
        return getChildrenRecursive(false);
    }

    @Override
    public Collection<GComponentClass> getChildrenRecursive(boolean returnSelf) {
        return getRelatedGNodeCollection(
                GComponentClass.class,
                GLinkType.CLASS_TO_PARENT,
                Direction.INCOMING,
                StopEvaluator.END_OF_GRAPH,
                (returnSelf ? ReturnableEvaluator.ALL : ReturnableEvaluator.ALL_BUT_START_NODE));
    }

    @Override
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

    @Override
    public boolean isHasComponentsRecursive() {
        for (GComponentClass cc: getChildrenRecursive(true)) {
            if (cc.isHasComponents()) {
                return true;
            }
        }
        return false;
    }

}
