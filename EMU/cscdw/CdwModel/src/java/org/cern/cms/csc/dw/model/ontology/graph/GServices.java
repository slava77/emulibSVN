package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;

public abstract class GServices extends GServicesBase {

    public GServices(String path) {
        super(path);
    }

    public GComponentClass getGComponentClass(ComponentClassType type) {
        return getGNodeByProperty(GComponentClass.class, GNode.PropertyType.TYPE, type.getValue());
    }

    public GComponentLinkClass getGComponentLinkClass(ComponentLinkClassType type) {
        return getGNodeByProperty(GComponentLinkClass.class, GNode.PropertyType.TYPE, type.getValue());
    }

    @SuppressWarnings("unchecked")
    public Collection<GComponentClass> getGComponentClasses() {
        GQuery q = new GQuery(this, GComponentClass.class);
        q.addGLinkTypeDirection(GLinkType.TYPE_COMPONENT_CLASS, Direction.OUTGOING);
        q.setStop(StopEvaluator.DEPTH_ONE);
        q.setReturnable(ReturnableEvaluator.ALL_BUT_START_NODE);
        return q.getRelatedGNodeCollection(getReferenceNode());
    }

    @SuppressWarnings("unchecked")
    public Collection<GComponentLinkClass> getGComponentLinkClasses() {
        GQuery q = new GQuery(this, GComponentLinkClass.class);
        q.addGLinkTypeDirection(GLinkType.TYPE_COMPONENT_LINK_CLASS, Direction.OUTGOING);
        q.setStop(StopEvaluator.DEPTH_ONE);
        q.setReturnable(ReturnableEvaluator.ALL_BUT_START_NODE);
        return q.getRelatedGNodeCollection(getReferenceNode());
    }

    @SuppressWarnings("unchecked")
    public Collection<GComponent> getGComponents() {
        GQuery q = new GQuery(this, GComponent.class);
        q.addGLinkTypeDirection(GLinkType.TYPE_COMPONENT, Direction.OUTGOING);
        q.setStop(StopEvaluator.DEPTH_ONE);
        q.setReturnable(ReturnableEvaluator.ALL_BUT_START_NODE);
        return q.getRelatedGNodeCollection(getReferenceNode());
    }

    @SuppressWarnings("unchecked")
    public <T extends GNode> T getGNodeById(Class<T> ifClass, Long id) {
        GQuery q = new GQuery(this, ifClass);
        return (T) q.getGNode(id);
    }

    @SuppressWarnings("unchecked")
    public <T extends GNode> Collection<T> getGNodesByProperty(Class<T> ifClass, GNode.PropertyType property, Object value) {
        GQuery q = new GQuery(this, ifClass);
        return q.getGNodes(property, value);
    }

    @SuppressWarnings("unchecked")
    public <T extends GNode> T getGNodeByProperty(Class<T> ifClass, GNode.PropertyType property, Object value) {
        GQuery q = new GQuery(this, ifClass);
        return (T) q.getGNode(property, value);
    }

    public <T extends GNode> Collection<T> getGNodesByPropertyFT(Class<T> ifClass, GNode.PropertyType property, String query) {
        return getGNodesByPropertyFT(ifClass, property, query, new DefaultNodeFilter(), 0);
    }

    @SuppressWarnings("unchecked")
    public <T extends GNode> Collection<T> getGNodesByPropertyFT(Class<T> ifClass, GNode.PropertyType property, String query, GNodeFilter filter, long numResults) {
        GQuery q = new GQuery(this, ifClass);
        q.setFilter(filter);
        q.setLimitResults(numResults);
        return q.getGNodesFT(property, query);
    }

}
