package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;
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
        Transaction tx = beginTx();
        try {
            Node node = getIdxSrv().getSingleNode(
                    GNodeImpl.getPropertyKey(GComponentClass.class, GNode.PropertyType.TYPE),
                    type.value());
            if (node != null) {
                    return new GComponentClassImpl(this, node);
            }
        }
        catch (ClassNotFoundException ex) {
            Logger.getLogger(GServices.class.getName()).log(Level.SEVERE, null, ex);
        }
        finally {
            tx.success();
            tx.finish();
        }
        return null;
    }

    public Collection<GComponentClass> getGComponentClasses() {
        return GUtility.getRelatedGNodeCollection(
                this,
                getReferenceNode(),
                GComponentClass.class,
                GComponentClassImpl.class,
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
                GComponentLinkClassImpl.class,
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
                GComponentImpl.class,
                GLinkType.TYPE_COMPONENT,
                Direction.OUTGOING,
                StopEvaluator.DEPTH_ONE,
                ReturnableEvaluator.ALL_BUT_START_NODE);
    }

    public <T extends GNode> Collection<T> getGNodesByProperty(Class<T> ifClass, Class implClass, GNode.PropertyType property, Object value) {
        Transaction tx = beginTx();
        try {
            return GUtility.wrap(this,
                    ifClass,
                    implClass,
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

    public <T extends GNode> T getGNodeByProperty(Class<T> ifClass, Class implClass, GNode.PropertyType property, Object value) {
        Transaction tx = beginTx();
        try {
            String pname = GNodeImpl.getPropertyKey(ifClass, property);
            return GUtility.wrap(
                    this,
                    ifClass,
                    implClass,
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

    public <T extends GNode> Collection<T> getGNodesByPropertyFT(Class<T> ifClass, Class implClass, GNode.PropertyType property, String query) {
        return getGNodesByPropertyFT(ifClass, implClass, property, query, 0);
    }

    public <T extends GNode> Collection<T> getGNodesByPropertyFT(Class<T> ifClass, Class implClass, GNode.PropertyType property, String query, long numResults) {
        Transaction tx = beginTx();
        try {

            return GUtility.wrap(this,
                    ifClass,
                    implClass,
                    getFTQueryIdxSrv().getNodes(
                        GNodeImpl.getPropertyKey(ifClass, property),
                        query,
                        Sort.RELEVANCE),
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
