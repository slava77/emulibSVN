package org.cern.cms.csc.dw.model.ontology.graph;

import java.lang.reflect.Constructor;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
import org.neo4j.graphdb.Transaction;
import org.neo4j.graphdb.Traverser;

public class GUtility {

    @SuppressWarnings("unchecked")
    public static <T extends GNode> T wrap(GServices gservices, Class<T> ifClass, Class implClass, Node node) {
        try {
            if (node != null) {
                Constructor constr = implClass.getConstructor(GServices.class, Node.class);
                return (T) constr.newInstance(gservices, node);
            } else {
                return null;
            }
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        }
    }

    /**
     * Wrap traverser output to the collection of Objects (static method)
     * @param <T> Type to wrap objects to
     * @param gservices Graph database services
     * @param ifClass Interface class of collection objects
     * @param implClass Implementation class of collection objects
     * @param traverser Traverser to process
     * @return Collection
     */
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Class implClass, Traverser traverser) {
        return wrap(gservices, ifClass, implClass, traverser.iterator());
    }

    /**
     * Wrap all iterator items to Object collection (static method)
     * @param <T> Type to wrap objects to
     * @param gservices Graph database services
     * @param ifClass Interface class of collection objects
     * @param implClass Implementation class of collection objects
     * @param it Iterator to process
     * @return Collection
     */
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Class implClass, Iterator<Node> it) {
        return wrap(gservices, ifClass, implClass, it, new DefaultNodeFilter(), 0);
    }

    /**
     * Wrap iterator limitResults items to Object collection (static method)
     * @param <T>
     * @param gservices
     * @param ifClass
     * @param implClass
     * @param it
     * @param limitResults results to limit or 0 for all
     * @return
     */
    @SuppressWarnings("unchecked")
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Class implClass, Iterator<Node> it, GNodeFilter filter, long limitResults) {
        long c = limitResults;
        try {
            
            Constructor constr = implClass.getConstructor(GServices.class, Node.class);
            Collection<T>  ret = new LinkedList<T>();

            // If limiting results
            if (limitResults > 0) {

                while (it.hasNext()) {
                    T gnode = (T) constr.newInstance(gservices, it.next());
                    if (filter.filter(gnode)) {
                        ret.add(gnode);
                        if (c == 0) {
                            break;
                        }
                        c -= 1;
                    }
                }

            // If all to be returned
            } else {

                while (it.hasNext()) {
                    T gnode = (T) constr.newInstance(gservices, it.next());
                    if (filter.filter(gnode)) {
                        ret.add(gnode);
                    }
                }
                
            }
            
            return ret;
            
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        }
    }

    /**
     * Get GNode object by using some relationship
     * @param <T>
     * @param ifClass
     * @param implClass
     * @param type
     * @param dir
     * @return
     */
    @SuppressWarnings("unchecked")
    public static <T extends GNode> T getRelatedGNode(GServices gservices, Node node, Class<T> ifClass, Class implClass, GLinkType type, Direction dir) {
        Transaction tx = gservices.beginTx();
        try {
            Constructor constr = implClass.getConstructor(GServices.class, Node.class);
            Relationship rel = node.getSingleRelationship(type, dir);
            if (rel != null) {
                return (T) constr.newInstance(gservices, rel.getEndNode());
            } else {
                return null;
            }
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public static <T extends GNode> Collection<T> getRelatedGNodeCollection(GServices gservices, Node node, Class<T> ifClass, Class implClass, GLinkType type, Direction dir, StopEvaluator stop, ReturnableEvaluator returnable) {
        Transaction tx = gservices.beginTx();
        try {
            return wrap(
                gservices,
                ifClass,
                implClass,
                node.traverse(
                    Traverser.Order.BREADTH_FIRST,
                    stop,
                    returnable,
                    type,
                    dir)
                );
        } finally {
            tx.success();
            tx.finish();
        }
    }

}
