package org.cern.cms.csc.dw.model.ontology.graph;

import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import org.apache.lucene.search.Sort;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.RelationshipType;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
import org.neo4j.graphdb.Transaction;
import org.neo4j.graphdb.Traverser;
import org.neo4j.graphdb.Traverser.Order;

public class GQuery <T extends GNode> {

    private static Logger logger = ExsysLogger.getLogger(GQuery.class);

    private final GServices gservices;
    private final Class<T> ifClass;
    private Constructor constructor;

    private GLinkTypesDirections linkTypesDirections = new GLinkTypesDirections();

    private StopEvaluator stop = StopEvaluator.DEPTH_ONE;
    private ReturnableEvaluator returnable = ReturnableEvaluator.ALL;
    private Traverser.Order order = Traverser.Order.DEPTH_FIRST;
    private GNodeFilter filter = new DefaultNodeFilter();
    private long limitResults = 0;
    private Sort sort = Sort.RELEVANCE;

    @SuppressWarnings("unchecked")
    public GQuery(GServices gservices, Class<T> ifClass) {
        this.gservices = gservices;
        this.ifClass = ifClass;
        Class implClass = GBase.implClass(ifClass);
        try {
            this.constructor = implClass.getConstructor(GServices.class, Node.class);
        } catch (Exception ex) {
            logger.error(null, ex);
        }
    }

    public void addGLinkTypeDirection(RelationshipType type, Direction dir) {
        linkTypesDirections.add(type, dir);
    }

    public void setReturnable(ReturnableEvaluator returnable) {
        this.returnable = returnable;
    }

    public void setStop(StopEvaluator stop) {
        this.stop = stop;
    }

    public void setFilter(GNodeFilter filter) {
        this.filter = filter;
    }

    public void setLimitResults(long limitResults) {
        this.limitResults = limitResults;
    }

    public void setOrder(Order order) {
        this.order = order;
    }

    public void setSort(Sort sort) {
        this.sort = sort;
    }

    @SuppressWarnings("unchecked")
    public T wrap(Node node) {
        try {
            
            if (node != null && isGNodeObjectType(node)) {
                return (T) constructor.newInstance(gservices, node);
            }
            
            return (T) null;

        } catch (Exception e) {
            logger.error(null, e);
            return null;
        }
    }

    public Collection<T> wrap(Traverser t) {
        return wrap(t.iterator());
    }

    public Collection<T> wrap(Iterator<Node> it) {
        long c = limitResults;
        try {

            Collection<T>  ret = new LinkedList<T>();

            // If limiting results
            if (c > 0) {

                while (it.hasNext()) {
                    Node n = it.next();
                    if (isGNodeObjectType(n)) {
                        T gnode = wrap(n);
                        if (gnode != null && filter.filter(gnode)) {
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
                    if (isGNodeObjectType(n)) {
                        T gnode = wrap(n);
                        if (gnode != null && filter.filter(gnode)) {
                            ret.add(gnode);
                        }
                    }
                }

            }

            return ret;

        } catch (Exception e) {
            logger.error(null, e);
            return null;
        }
    }

    public Iterator<T> wrapIterator(final Iterator<Node> it) {

        return new Iterator<T>() {
            
            private final Iterator<Node> iterator = it;

            public boolean hasNext() {
                return iterator.hasNext();
            }

            public T next() {
                return wrap(iterator.next());
            }

            public void remove() {
                iterator.remove();
            }
            
        };
        
    }

    public T getGNode(Long id) {
        Transaction tx = gservices.beginTx();
        try {
            return wrap(gservices.getDBSrv().getNodeById(id));
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    public Collection<T> getGNodes(GNode.PropertyType property, Object value) {
        Transaction tx = gservices.beginTx();
        try {
            return wrap(gservices.getIdxSrv().getNodes(
                    GNodeImpl.getPropertyKey(ifClass, property),
                    value));
        } catch (Exception e) {
            logger.error(null, e);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    public T getGNode(GNode.PropertyType property, Object value) {
        Transaction tx = gservices.beginTx();
        try {
            return wrap(gservices.getIdxSrv().getSingleNode(
                    GNodeImpl.getPropertyKey(ifClass, property),
                    value));

        } catch (Exception e) {
            logger.error(null, e);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    public Collection<T> getGNodesFT(GNode.PropertyType property, String query) {
        Transaction tx = gservices.beginTx();
        try {

            return wrap(
                    gservices.getFTQueryIdxSrv().getNodes(
                        GNodeImpl.getPropertyKey(ifClass, property),
                        query,
                        sort));

        } catch (Exception e) {
            logger.error(null, e);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    @SuppressWarnings("unchecked")
    public T getRelatedGNode(Node node) {
        Transaction tx = gservices.beginTx();
        try {
            
            if (linkTypesDirections.notEmpty()) {
                Relationship rel = node.getSingleRelationship(
                        linkTypesDirections.getFirst().getType(),
                        linkTypesDirections.getFirst().getDir());
                if (rel != null) {
                    T gnode = wrap(rel.getOtherNode(node));
                    if (gnode != null && filter.filter(gnode)) {
                        return gnode;
                    }
                }
            }

            return (T) null;

        } finally {
            tx.success();
            tx.finish();
        }
    }

    @SuppressWarnings("unchecked")
    public Collection<T> getRelatedGNodeCollection(Node node) {
        Transaction tx = gservices.beginTx();
        try {

            if (linkTypesDirections.notEmpty()) {
                return wrap(
                    node.traverse(
                        order,
                        stop,
                        returnable,
                        linkTypesDirections.getAsArray())
                    );
            }

            return Collections.EMPTY_LIST;

        } finally {
            tx.success();
            tx.finish();
        }
    }


    @SuppressWarnings("unchecked")
    public boolean isGNodeObjectType(Node n) {
        Transaction tx = gservices.beginTx();
        try {
            return n.getProperty(GNode.KEY_OBJECT_TYPE, "").equals(ifClass.getName());
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    private class GLinkTypesDirections {

        private List<GLinkTypeDirection> values = new ArrayList<GLinkTypeDirection>();

        public void add(RelationshipType type, Direction dir) {
            this.values.add(new GLinkTypeDirection(type, dir));
        }

        public boolean notEmpty() {
            return ! this.values.isEmpty();
        }

        public GLinkTypeDirection getFirst() {
            if (notEmpty()) {
                return this.values.get(0);
            } else {
                return null;
            }
        }

        public Object [] getAsArray() {
            Object [] arr = new Object[values.size() * 2];
            int i = 0;
            for (GLinkTypeDirection o: values) {
                arr[i++] = o.getType();
                arr[i++] = o.getDir();
            }
            return arr;
        }

        private class GLinkTypeDirection implements Comparable {

            private RelationshipType type;
            private Direction dir;

            public GLinkTypeDirection(RelationshipType type, Direction dir) {
                this.type = type;
                this.dir = dir;
            }

            public Direction getDir() {
                return dir;
            }

            public RelationshipType getType() {
                return type;
            }

            public int compareTo(Object o) {
                if (o.getClass().equals(this.getClass())) {
                    GLinkTypeDirection link = (GLinkTypeDirection) o;
                    return (link.getDir().equals(getDir()) && link.getType().equals(getType()) ? 0 : 1);
                }
                return 1;
            }

        }
        
    }

}
