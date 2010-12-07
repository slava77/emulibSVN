package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.apache.log4j.Logger;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.RelationshipType;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
import org.neo4j.graphdb.Transaction;

public abstract class GNodeImpl extends GBase implements GNode {

    private static Logger logger = Logger.getLogger(GNodeImpl.class);
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
            logger.log(null, ex);
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

    protected  <T extends GNode> GQuery<T> getGQuery(Class<T> ifClass) {
        return new GQuery<T>(gservices, ifClass);
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

    protected Object getProperty(PropertyType prop, Object defaultValue) {
        Transaction tx = gservices.beginTx();
        try {
            return node.getProperty(getPropertyKey(prop), defaultValue);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    protected void setRelationship(GNode toNode, RelationshipType type) {
        setRelationship(toNode.getNode(), type);
    }

    protected void setRelationship(Node toNode, RelationshipType type) {
        Transaction tx = gservices.beginTx();
        try {
            node.createRelationshipTo(toNode, type);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    @SuppressWarnings("unchecked")
    protected <T extends GNode> T getRelatedGNode(Class<T> ifClass, RelationshipType type, Direction dir) {
        GQuery q = new GQuery(gservices, ifClass);
        q.addGLinkTypeDirection(type, dir);
        return (T) q.getRelatedGNode(node);
    }

    protected <T extends GNode> Collection<T> getRelatedGNodeCollection(Class<T> ifClass, RelationshipType type, Direction dir) {
        return getRelatedGNodeCollection(ifClass, type, dir, StopEvaluator.DEPTH_ONE, ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    @SuppressWarnings("unchecked")
    protected <T extends GNode> Collection<T> getRelatedGNodeCollection(Class<T> ifClass, RelationshipType type, Direction dir, StopEvaluator stop, ReturnableEvaluator returnable) {
        GQuery q = new GQuery(gservices, ifClass);
        q.addGLinkTypeDirection(type, dir);
        q.setStop(stop);
        q.setReturnable(returnable);
        return q.getRelatedGNodeCollection(node);
    }

    @SuppressWarnings("unchecked")
    protected <T extends GNode> Collection<T> getRelatedGNodeCollection(Class<T> ifClass, RelationshipType type1, Direction dir1, RelationshipType type2, Direction dir2, StopEvaluator stop, ReturnableEvaluator returnable) {
        GQuery q = new GQuery(gservices, ifClass);
        q.addGLinkTypeDirection(type1, dir1);
        q.addGLinkTypeDirection(type2, dir2);
        q.setStop(stop);
        q.setReturnable(returnable);
        return q.getRelatedGNodeCollection(node);
    }

    public int compareTo(Object o) {
        if (o instanceof GNode) {
            return this.getId().compareTo(((GNode) o).getId());
        }
        return 2;
    }

}
