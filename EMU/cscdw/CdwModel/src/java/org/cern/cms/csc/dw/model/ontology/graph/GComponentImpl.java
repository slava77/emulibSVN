package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.LinkedHashSet;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;

public class GComponentImpl extends GNodeImpl implements GComponent {

    public GComponentImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    public void setName(String name) {
        setProperty(PropertyType.NAME, name, true);
    }

    public String getName() {
        return (String) getProperty(PropertyType.NAME);
    }

    public void setDescription(String description) {
        setProperty(PropertyType.DESCRIPTION, description);
    }

    public String getDescription() {
        return (String) getProperty(PropertyType.DESCRIPTION, null);
    }

    public void setType(GComponentClass type) {
        setRelationship(type, GLinkType.COMPONENT_TO_CLASS);
    }

    public GComponentClass getType() {
        return getRelatedGNode(
                GComponentClass.class,
                GLinkType.COMPONENT_TO_CLASS,
                Direction.OUTGOING);
    }

    public void addSynonym(GComponentSynonym synonym) {
        if (!getSynonyms().contains(synonym) && synonym.getComponent() == null) {
            setRelationship(synonym, GLinkType.COMPONENT_TO_SYNONYM);
        }
    }

    public Collection<GComponentSynonym> getSynonyms() {
        return getRelatedGNodeCollection(
                GComponentSynonym.class,
                GLinkType.COMPONENT_TO_SYNONYM,
                Direction.OUTGOING);
    }

    public boolean isSetType() {
        return (getType() != null);
    }

    public GComponentLinks getGComponentLinks() {
        return getRelatedGNode(
                GComponentLinks.class,
                GLinkType.COMPONENT_TO_LINKS,
                Direction.OUTGOING);
    }

    public Collection<GComponentLink> getLinks() {
        return getGComponentLinks().getLinks();
    }

    public boolean isSetName() {
        return (getName() != null);
    }

    public Collection<GComponentClass> getTypes() {
        return getType().getParentsRecursive(true);
    }

    public Collection<GComponent> getRelatedGComponents(ComponentLinkClassType type, Direction dir) {
        Collection<GComponent> ret = new LinkedHashSet<GComponent>();

        if (dir.equals(Direction.OUTGOING) || dir.equals(Direction.BOTH)) {
            ret.addAll(getGComponentLinks().getRelatedGComponents(type));
        }

        if (dir.equals(Direction.INCOMING) || dir.equals(Direction.BOTH)) {
            for (GComponentLinks links: getRelatedGNodeCollection(
                                            GComponentLinks.class,
                                            type, Direction.INCOMING)) {
                ret.add(links.getGComponent());
            }
        }

        return ret;
    }

    public boolean hasRelatedGComponents(ComponentLinkClassType type, Direction dir) {
        boolean ret = false;

        if (dir.equals(Direction.OUTGOING) || dir.equals(Direction.BOTH)) {
            ret = getGComponentLinks().hasLink(type);
        }

        if (!ret && (dir.equals(Direction.INCOMING) || dir.equals(Direction.BOTH))) {
            ret = ! getRelatedGNodeCollection(GComponentLinks.class, type, Direction.INCOMING).isEmpty();
        }

        return ret;
    }

    public Collection<GComponent> getRelatedGComponents(ComponentLinkClassType type) {
        return getRelatedGComponents(type, Direction.OUTGOING);
    }

    public boolean hasRelatedGComponents(ComponentLinkClassType type) {
        return hasRelatedGComponents(type, Direction.OUTGOING);
    }

}
