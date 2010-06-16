package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.Iterator;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
import org.neo4j.graphdb.Traverser;

public class GUtility {

    @SuppressWarnings("unchecked")
    public static <T extends GNode> T wrap(GServices gservices, Class<T> ifClass, Node node) {
        GQuery q = new GQuery<T>(gservices, ifClass);
        return (T) q.wrap(node);
    }

    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Traverser traverser) {
        return wrap(gservices, ifClass, traverser.iterator());
    }

    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Iterator<Node> it) {
        return wrap(gservices, ifClass, it, new DefaultNodeFilter(), 0);
    }

    @SuppressWarnings("unchecked")
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Iterator<Node> it, GNodeFilter filter, long limitResults) {
        GQuery q = new GQuery<T>(gservices, ifClass);
        q.setFilter(filter);
        q.setLimitResults(limitResults);
        return q.wrap(it);
    }

    @SuppressWarnings("unchecked")
    public static <T extends GNode> T getRelatedGNode(GServices gservices, Node node, Class<T> ifClass, GLinkType type, Direction dir) {
        GQuery q = new GQuery<T>(gservices, ifClass);
        q.addGLinkTypeDirection(type, dir);
        return (T) q.getRelatedGNode(node);
    }

    @SuppressWarnings("unchecked")
    public static <T extends GNode> Collection<T> getRelatedGNodeCollection(GServices gservices, Node node, Class<T> ifClass, GLinkType type, Direction dir, StopEvaluator stop, ReturnableEvaluator returnable) {
        GQuery q = new GQuery<T>(gservices, ifClass);
        q.setStop(stop);
        q.setReturnable(returnable);
        q.addGLinkTypeDirection(type, dir);
        return q.getRelatedGNodeCollection(node);
    }

    @SuppressWarnings("unchecked")
    public static <T extends GNode> Collection<T> getRelatedGNodeCollection(GServices gservices, Node node, Class<T> ifClass, GLinkType type1, Direction dir1, GLinkType type2, Direction dir2, StopEvaluator stop, ReturnableEvaluator returnable) {
        GQuery q = new GQuery<T>(gservices, ifClass);
        q.setStop(stop);
        q.setReturnable(returnable);
        q.addGLinkTypeDirection(type1, dir1);
        q.addGLinkTypeDirection(type2, dir2);
        return q.getRelatedGNodeCollection(node);
    }

}
