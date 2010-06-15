package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
import org.neo4j.graphdb.Transaction;
import org.neo4j.graphdb.Traverser;

public abstract class GNodeImpl extends GBase implements GNode {

    protected final Node node;

    /**
     * Constructor
     * @param gservices Graph Database Services
     * @param node Base node for the object
     */
    public GNodeImpl(GServices gservices, Node node) {
        super(gservices);
        this.node = node;
    }

    public String getPropertyKey(PropertyType type) {
        try {
            return getPropertyKey(this.getClass(), type);
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(GNodeImpl.class.getName()).log(Level.SEVERE, null, ex);
            return null;
        }
    }

    public static String getPropertyKey(Class<? extends GNode> clazz, PropertyType type) throws ClassNotFoundException {
        Class ifclazz = clazz;

        if (!ifclazz.isInterface()) {
            ifclazz = GBase.ifClass(ifclazz);
        }

        return ifclazz.getName().concat(".").concat(type.name());
    }

    public Long getId() {
        return node.getId();
    }

    public Node getNode() {
        return node;
    }

    protected <T extends GNode> Collection<T> wrap(Class<T> ifClass, Traverser traverser) {
        return wrap(ifClass, traverser.iterator());
    }

    protected <T extends GNode> Collection<T> wrap(Class<T> ifClass, Iterator<Node> it) {
        return GUtility.wrap(gservices, ifClass, it);
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof GNodeImpl) {
            return node.equals(((GNodeImpl) obj).getNode());
        }
        return false;
    }

    @Override
    public int hashCode() {
        return node.hashCode();
    }

    public void setProperty(PropertyType prop, Object value) {
        setProperty(prop, value, false);
    }

    public void setProperty(PropertyType prop, Object value, boolean indexMe) {
        Transaction tx = gservices.beginTx();
        try {
            node.setProperty(getPropertyKey(prop), value);
            if (indexMe) {
                gservices.getIdxSrv().index(node, getPropertyKey(prop), value);
                gservices.getFTIdxSrv().index(node, getPropertyKey(prop), value);
            }
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public void removeProperty(PropertyType prop) {
        Transaction tx = gservices.beginTx();
        try {
            if (node.hasProperty(getPropertyKey(prop))) {
                gservices.getIdxSrv().removeIndex(node, getPropertyKey(prop), getProperty(prop));
                gservices.getFTIdxSrv().removeIndex(node, getPropertyKey(prop), getProperty(prop));
                node.removeProperty(getPropertyKey(prop));
            }
        } finally {
            tx.success();
            tx.finish();
        }
    }

    protected Object getProperty(PropertyType prop) {
        Transaction tx = gservices.beginTx();
        try {
            return node.getProperty(getPropertyKey(prop));
        } finally {
            tx.success();
            tx.finish();
        }
    }

    /**
     * Get graph node property. If not exists return default value
     * @param name
     * @param defaultValue
     * @return
     */
    protected Object getProperty(PropertyType prop, Object defaultValue) {
        Transaction tx = gservices.beginTx();
        try {
            return node.getProperty(getPropertyKey(prop), defaultValue);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    /**
     * Create relationship to some object
     * @param toNode
     * @param type
     */
    protected void setRelationship(GNode toNode, GLinkType type) {
        setRelationship(toNode.getNode(), type);
    }

    /**
     * Create relationship to some node
     * @param toNode
     * @param type
     */
    protected void setRelationship(Node toNode, GLinkType type) {
        Transaction tx = gservices.beginTx();
        try {
            node.createRelationshipTo(toNode, type);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    @SuppressWarnings("unchecked")
    protected <T extends GNode> T getRelatedGNode(Class<T> ifClass, GLinkType type, Direction dir) {
        return GUtility.getRelatedGNode(gservices, node, ifClass, type, dir);
    }

    protected <T extends GNode> Collection<T> getRelatedGNodeCollection(Class<T> ifClass, GLinkType type, Direction dir) {
        return getRelatedGNodeCollection(
                    ifClass,
                    type,
                    dir,
                    StopEvaluator.DEPTH_ONE,
                    ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    protected <T extends GNode> Collection<T> getRelatedGNodeCollection(Class<T> ifClass, GLinkType type, Direction dir, StopEvaluator stop, ReturnableEvaluator returnable) {
        return GUtility.getRelatedGNodeCollection(
                gservices,
                node,
                ifClass,
                type,
                dir,
                stop,
                returnable);
    }

    protected <T extends GNode> Collection<T> getRelatedGNodeCollection(Class<T> ifClass, StopEvaluator stop, ReturnableEvaluator returnable, Object... typesAndDirections) {
        return GUtility.getRelatedGNodeCollection(
                gservices,
                node,
                ifClass,
                stop,
                returnable,
                typesAndDirections);
    }

    public int compareTo(Object o) {
        if (o instanceof GNode) {
            return this.getId().compareTo(((GNode) o).getId());
        }
        return 2;
    }

}
