package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;

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

    public Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType, Direction dir) {
        Collection<GComponent> ret = new LinkedHashSet<GComponent>();

        if (dir.equals(Direction.OUTGOING) || dir.equals(Direction.BOTH)) {
            ret.addAll(getGComponentLinks().getRelatedGComponents(linkType));
        }

        if (dir.equals(Direction.INCOMING) || dir.equals(Direction.BOTH)) {
            for (GComponentLinks links: getRelatedGNodeCollection(
                                            GComponentLinks.class,
                                            linkType, Direction.INCOMING)) {
                ret.add(links.getGComponent());
            }
        }

        return ret;
    }

    public boolean hasRelatedGComponents(ComponentLinkClassType linkType, Direction dir) {
        boolean ret = false;

        if (dir.equals(Direction.OUTGOING) || dir.equals(Direction.BOTH)) {
            ret = getGComponentLinks().hasLink(linkType);
        }

        if (!ret && (dir.equals(Direction.INCOMING) || dir.equals(Direction.BOTH))) {
            ret = ! getRelatedGNodeCollection(GComponentLinks.class, linkType, Direction.INCOMING).isEmpty();
        }

        return ret;
    }

    public Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType) {
        return getRelatedGComponents(linkType, Direction.OUTGOING);
    }

    public boolean hasRelatedGComponents(ComponentLinkClassType linkType) {
        return hasRelatedGComponents(linkType, Direction.OUTGOING);
    }

    public Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType, GComponentClass type) {
        Collection<GComponent> ret = new LinkedList<GComponent>();
        for (GComponent gc: findRelatedGComponents(linkType)) {
            if (gc.getType().equals(type)) {
                ret.add(gc);
            }
        }
        return ret;
    }

    public Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType) {
        return getRelatedGNodeCollection(
                GComponent.class,
                GLinkType.COMPONENT_TO_LINKS,
                Direction.OUTGOING,
                linkType,
                Direction.OUTGOING,
                StopEvaluator.END_OF_GRAPH,
                ReturnableEvaluator.ALL_BUT_START_NODE);
    }

}
