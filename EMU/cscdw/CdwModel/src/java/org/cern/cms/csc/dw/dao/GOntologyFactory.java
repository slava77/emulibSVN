package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClassImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClassImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinks;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonym;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonymImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GLinkType;
import org.cern.cms.csc.dw.model.ontology.graph.GNode;
import org.cern.cms.csc.dw.model.ontology.graph.GNodeImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GraphDevServices;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.Transaction;

public class GOntologyFactory {

    private final GServices gservices;
    private Transaction gtx;

    public GOntologyFactory() {
         gservices = new GraphDevServices();
         gtx = gservices.beginTx();
    }

    public void intermediateCommit() {
        gtx.success();
        gtx.finish();
        gtx = gservices.beginTx();
    }

    public void close() {
        gtx.success();
        gtx.finish();
        gservices.close();
    }

    public Collection<GComponentClass> getGComponentClasses() {
        return gservices.getGComponentClasses();
    }

    public Collection<GComponentLinkClass> getGComponentLinkClasses() {
        return gservices.getGComponentLinkClasses();
    }

    public Collection<GComponent> getGComponents() {
        return gservices.getGComponents();
    }

    public GComponentClass createGComponentClass() {
        Transaction tx = gservices.beginTx();
        try {
            Node node = gservices.getDBSrv().createNode();
            gservices.getDBSrv().getReferenceNode().createRelationshipTo(node, GLinkType.TYPE_COMPONENT_CLASS);
            setObjectType(node, GComponentClass.class);
            return new GComponentClassImpl(gservices, node);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public GComponentLinkClass createGComponentLinkClass() {
        Transaction tx = gservices.beginTx();
        try {
            Node node = gservices.getDBSrv().createNode();
            gservices.getDBSrv().getReferenceNode().createRelationshipTo(node, GLinkType.TYPE_COMPONENT_LINK_CLASS);
            setObjectType(node, GComponentLinkClass.class);
            return new GComponentLinkClassImpl(gservices, node);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public GComponent createGComponent() {
        Transaction tx = gservices.beginTx();
        try {

            Node node = gservices.getDBSrv().createNode();
            gservices.getDBSrv().getReferenceNode().createRelationshipTo(node, GLinkType.TYPE_COMPONENT);
            setObjectType(node, GComponent.class);

            Node linksNode = gservices.getDBSrv().createNode();
            node.createRelationshipTo(linksNode, GLinkType.COMPONENT_TO_LINKS);
            setObjectType(node, GComponentLinks.class);
            
            return new GComponentImpl(gservices, node);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public void removeGComponent(GComponent gc) {
        Transaction tx = gservices.beginTx();
        try {

            // Removing relationships
            for (Relationship r: gc.getNode().getRelationships()) {
                r.delete();
            }

            // Removing properties and coresponding indexes
            ((GNodeImpl) gc).removeProperty(GNode.PropertyType.NAME);
            ((GNodeImpl) gc).removeProperty(GNode.PropertyType.DESCRIPTION);

            // Removing node
            gc.getNode().delete();

        } finally {
            tx.success();
            tx.finish();
        }
    }

    public GComponentSynonym createGComponentSynonym() {
        Transaction tx = gservices.beginTx();
        try {
            Node node = gservices.getDBSrv().createNode();
            gservices.getDBSrv().getReferenceNode().createRelationshipTo(node, GLinkType.TYPE_COMPONENT_SYNONYM);
            setObjectType(node, GComponentSynonym.class);
            return new GComponentSynonymImpl(gservices, node);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    private void setObjectType(Node node, Class<? extends GNode> clazz) {
        node.setProperty(GNode.KEY_OBJECT_TYPE, clazz.getName());
        gservices.getIdxSrv().index(node, GNode.KEY_OBJECT_TYPE, clazz.getName());
    }

}
