package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.Iterator;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.Transaction;

public class GComponentLinksImpl extends GNodeImpl implements GComponentLinks {

    public GComponentLinksImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    public void addLink(GComponent component, ComponentLinkClassType type) {
        Transaction tx = gservices.beginTx();
        try {
            if (!hasLink(component, type)) {
                node.createRelationshipTo(component.getNode(), type);
            }
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public Collection<GComponentLink> getLinks() {
        Transaction tx = gservices.beginTx();
        try {
            return GLinkImpl.wrap(
                    gservices,
                    GComponentLink.class,
                    GComponentLinkImpl.class,
                    node.getRelationships(Direction.OUTGOING).iterator());
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public boolean hasLink(GComponent component, ComponentLinkClassType type) {
        Transaction tx = gservices.beginTx();
        try {
            Iterator<Relationship> it = node.getRelationships(type, Direction.OUTGOING).iterator();
            while (it.hasNext()) {
                if (it.next().getEndNode().getId() == component.getId()) {
                    return true;
                }
            }
        } finally {
            tx.success();
            tx.finish();
        }
        return false;
    }

    public boolean hasLink(ComponentLinkClassType type) {
        Transaction tx = gservices.beginTx();
        try {
            return node.hasRelationship(type, Direction.OUTGOING);
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public GComponent getGComponent() {
        return getRelatedGNode(GComponent.class, GLinkType.COMPONENT_TO_LINKS, Direction.INCOMING);
    }

    public Collection<GComponent> getRelatedGComponents(ComponentLinkClassType type) {
        return getRelatedGNodeCollection(GComponent.class, type, Direction.OUTGOING);
    }

}
