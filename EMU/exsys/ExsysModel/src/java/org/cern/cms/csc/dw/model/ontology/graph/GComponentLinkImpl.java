package org.cern.cms.csc.dw.model.ontology.graph;

import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.Transaction;

public class GComponentLinkImpl extends GLinkImpl implements GComponentLink {

    public GComponentLinkImpl(GServices gservices, Relationship relationship) {
        super(gservices, relationship);
    }

    public GComponent getComponent() {
        Transaction tx = gservices.beginTx();
        try {
            return new GComponentImpl(gservices, relationship.getEndNode());
        } finally {
            tx.success();
            tx.finish();
        }
    }

    public ComponentLinkClassType getType() {
        Transaction tx = gservices.beginTx();
        try {
            return ComponentLinkClassType.valueOf(relationship.getType().name());
        } finally {
            tx.success();
            tx.finish();
        }
    }

    

}
