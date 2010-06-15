package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.apache.lucene.search.Sort;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;
import org.neo4j.graphdb.Transaction;

public abstract class GServices extends GServicesBase {

    public GServices(String path) {
        super(path);
    }

    public GComponentClass getGComponentClass(ComponentClassType type) {
        return getGNodeByProperty(GComponentClass.class, GNode.PropertyType.TYPE, type.getValue());
    }

    public Collection<GComponentClass> getGComponentClasses() {
        return GUtility.getRelatedGNodeCollection(
                this,
                getReferenceNode(),
                GComponentClass.class,
                GLinkType.TYPE_COMPONENT_CLASS,
                Direction.OUTGOING,
                StopEvaluator.DEPTH_ONE,
                ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    public Collection<GComponentLinkClass> getGComponentLinkClasses() {
        return GUtility.getRelatedGNodeCollection(
                this,
                getReferenceNode(),
                GComponentLinkClass.class,
                GLinkType.TYPE_COMPONENT_LINK_CLASS,
                Direction.OUTGOING,
                StopEvaluator.DEPTH_ONE,
                ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    public Collection<GComponent> getGComponents() {
        return GUtility.getRelatedGNodeCollection(
                this,
                getReferenceNode(),
                GComponent.class,
                GLinkType.TYPE_COMPONENT,
                Direction.OUTGOING,
                StopEvaluator.DEPTH_ONE,
                ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    public <T extends GNode> T getGNodeById(Class<T> ifClass, Long id) {
        Transaction tx = beginTx();
        try {
            return GUtility.wrap(this,
                    ifClass,
                    getDBSrv().getNodeById(id));
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    public <T extends GNode> Collection<T> getGNodesByProperty(Class<T> ifClass, GNode.PropertyType property, Object value) {
        Transaction tx = beginTx();
        try {
            return GUtility.wrap(this,
                    ifClass,
                    getIdxSrv().getNodes(
                        GNodeImpl.getPropertyKey(ifClass, property),
                        value));
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    public <T extends GNode> T getGNodeByProperty(Class<T> ifClass, GNode.PropertyType property, Object value) {
        Transaction tx = beginTx();
        try {
            String pname = GNodeImpl.getPropertyKey(ifClass, property);
            return GUtility.wrap(
                    this,
                    ifClass,
                    getIdxSrv().getSingleNode(
                        pname,
                        value)
                    );
            
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    public <T extends GNode> Collection<T> getGNodesByPropertyFT(Class<T> ifClass, GNode.PropertyType property, String query) {
        return getGNodesByPropertyFT(ifClass, property, query, new DefaultNodeFilter(), 0);
    }

    public <T extends GNode> Collection<T> getGNodesByPropertyFT(Class<T> ifClass, GNode.PropertyType property, String query, GNodeFilter filter, long numResults) {
        Transaction tx = beginTx();
        try {

            return GUtility.wrap(this,
                    ifClass,
                    getFTQueryIdxSrv().getNodes(
                        GNodeImpl.getPropertyKey(ifClass, property),
                        query,
                        Sort.RELEVANCE),
                    filter,
                    numResults);
            
        } catch (Exception e) {
            e.printStackTrace(System.err);
            return null;
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

    @SuppressWarnings("unchecked")
    public Class<? extends GNode> getGNodeType(Node n) throws ClassNotFoundException {
        Transaction tx = beginTx();
        try {
            return (Class<? extends GNode>) ClassLoader.getSystemClassLoader().loadClass((String) n.getProperty(GNode.KEY_OBJECT_TYPE));
        }
        finally {
            tx.success();
            tx.finish();
        }
    }

}
