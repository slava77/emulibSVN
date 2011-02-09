package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;

public class GComponentLinkClassImpl extends GNodeImpl implements GComponentLinkClass {

    public GComponentLinkClassImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    @Override
    public void setType(ComponentLinkClassType type) {
        setProperty(InternalPropertyType.TYPE, type.value(), true);
    }

    @Override
    public ComponentLinkClassType getType() {
        return ComponentLinkClassType.fromValue((String) getProperty(InternalPropertyType.TYPE));
    }

    @Override
    public void setDescription(String description) {
        setProperty(InternalPropertyType.DESCRIPTION, description);
    }

    @Override
    public String getDescription() {
        return (String) getProperty(InternalPropertyType.DESCRIPTION, null);
    }

    @Override
    public Boolean getTransitive() {
        return (Boolean) getProperty(InternalPropertyType.TRANSITIVE, false);
    }

    @Override
    public void setTransitive(Boolean transitive) {
        setProperty(InternalPropertyType.TRANSITIVE, transitive);
    }

    @Override
    public void setParent(GComponentLinkClass parent) {
        setRelationship(parent, GLinkType.LINK_TO_PARENT);
    }

    @Override
    public GComponentLinkClass getParent() {
        return getRelatedGNode(
                GComponentLinkClass.class,
                GLinkType.LINK_TO_PARENT,
                Direction.OUTGOING);
    }

    @Override
    public Collection<GComponentLinkClass> getChildren() {
        return getRelatedGNodeCollection(
            GComponentLinkClass.class,
            GLinkType.LINK_TO_PARENT,
            Direction.INCOMING);
    }

    @Override
    public void setInverse(GComponentLinkClass inverse) {
        setRelationship(inverse, GLinkType.LINK_TO_INVERSE);
    }

    @Override
    public GComponentLinkClass getInverse() {
        return getRelatedGNode(
                GComponentLinkClass.class,
                GLinkType.LINK_TO_INVERSE,
                Direction.OUTGOING);
    }

}
