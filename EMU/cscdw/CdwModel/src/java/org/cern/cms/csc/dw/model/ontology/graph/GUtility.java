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
    public static <T extends GNode> T wrap(GServices gservices, Class<T> ifClass, Node node) {
        try {
            if (node != null && isGNodeObjectType(gservices, node, ifClass)) {
                Class implClass = GBase.implClass(ifClass);
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
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Traverser traverser) {
        return wrap(gservices, ifClass, traverser.iterator());
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
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Iterator<Node> it) {
        return wrap(gservices, ifClass, it, new DefaultNodeFilter(), 0);
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
    public static <T extends GNode> Collection<T> wrap(GServices gservices, Class<T> ifClass, Iterator<Node> it, GNodeFilter filter, long limitResults) {
        long c = limitResults;
        try {
            
            Class implClass = GBase.implClass(ifClass);
            Constructor constr = implClass.getConstructor(GServices.class, Node.class);
            Collection<T>  ret = new LinkedList<T>();

            // If limiting results
            if (limitResults > 0) {

                while (it.hasNext()) {
                    Node n = it.next();
                    if (isGNodeObjectType(gservices, n, ifClass)) {
                        T gnode = (T) constr.newInstance(gservices, n);
                        if (filter.filter(gnode)) {
                            ret.add(gnode);
                            if (c == 0) {
                                break;
                            }
                            c -= 1;
                        }
                    }
                }

            // If all to be returned
            } else {

                while (it.hasNext()) {
                    Node n = it.next();
                    if (isGNodeObjectType(gservices, n, ifClass)) {
                        T gnode = (T) constr.newInstance(gservices, n);
                        if (filter.filter(gnode)) {
                            ret.add(gnode);
                        }
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
    public static <T extends GNode> T getRelatedGNode(GServices gservices, Node node, Class<T> ifClass, GLinkType type, Direction dir) {
        Transaction tx = gservices.beginTx();
        try {
            Class implClass = GBase.implClass(ifClass);
            Constructor constr = implClass.getConstructor(GServices.class, Node.class);
            Relationship rel = node.getSingleRelationship(type, dir);
            if (rel != null) {
                Node n = rel.getOtherNode(node);
                if (isGNodeObjectType(gservices, n, ifClass)) {
                    return (T) constr.newInstance(gservices, n);
                }
            }
            return null;
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public static <T extends GNode> Collection<T> getRelatedGNodeCollection(GServices gservices, Node node, Class<T> ifClass, GLinkType type, Direction dir, StopEvaluator stop, ReturnableEvaluator returnable) {
        Transaction tx = gservices.beginTx();
        try {
            return wrap(
                gservices,
                ifClass,
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

    public static <T extends GNode> Collection<T> getRelatedGNodeCollection(GServices gservices, Node node, Class<T> ifClass, StopEvaluator stop, ReturnableEvaluator returnable, Object... typesAndDirections) {
        Transaction tx = gservices.beginTx();
        try {
            return wrap(
                gservices,
                ifClass,
                node.traverse(
                    Traverser.Order.BREADTH_FIRST,
                    stop,
                    returnable,
                    typesAndDirections)
                );
        } finally {
            tx.success();
            tx.finish();
        }
    }


    @SuppressWarnings("unchecked")
    public static boolean isGNodeObjectType(GServices gservices, Node n, Class<? extends GNode> classToCheck) throws ClassNotFoundException {
        Transaction tx = gservices.beginTx();
        try {
            return n.getProperty(GNode.KEY_OBJECT_TYPE).equals(classToCheck.getName());
        }
        finally {
            tx.success();
            tx.finish();
        }
    }


}
