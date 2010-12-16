package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;

public class GComponentLinkClassImpl extends GNodeImpl implements GComponentLinkClass {

    public GComponentLinkClassImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    public void setType(ComponentLinkClassType type) {
        setProperty(PropertyType.TYPE, type.value(), true);
    }

    public ComponentLinkClassType getType() {
        return ComponentLinkClassType.fromValue((String) getProperty(PropertyType.TYPE));
    }

    public void setDescription(String description) {
        setProperty(PropertyType.DESCRIPTION, description);
    }

    public String getDescription() {
        return (String) getProperty(PropertyType.DESCRIPTION, null);
    }

    public Boolean getTransitive() {
        return (Boolean) getProperty(PropertyType.TRANSITIVE, false);
    }

    public void setTransitive(Boolean transitive) {
        setProperty(PropertyType.TRANSITIVE, transitive);
    }

    public void setParent(GComponentLinkClass parent) {
        setRelationship(parent, GLinkType.LINK_TO_PARENT);
    }

    public GComponentLinkClass getParent() {
        return getRelatedGNode(
                GComponentLinkClass.class,
                GLinkType.LINK_TO_PARENT,
                Direction.OUTGOING);
    }

    public Collection<GComponentLinkClass> getChildren() {
        return getRelatedGNodeCollection(
            GComponentLinkClass.class,
            GLinkType.LINK_TO_PARENT,
            Direction.INCOMING);
    }

    public void setInverse(GComponentLinkClass inverse) {
        setRelationship(inverse, GLinkType.LINK_TO_INVERSE);
    }

    public GComponentLinkClass getInverse() {
        return getRelatedGNode(
                GComponentLinkClass.class,
                GLinkType.LINK_TO_INVERSE,
                Direction.OUTGOING);
    }

}
