package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
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
        return getRelatedGNode(GComponentClass.class, GComponentClassImpl.class, GLinkType.COMPONENT_TO_CLASS, Direction.OUTGOING);
    }

    public void addSynonym(GComponentSynonym synonym) {
        if (!getSynonyms().contains(synonym) && synonym.getComponent() == null) {
            setRelationship(synonym, GLinkType.COMPONENT_TO_SYNONYM);
        }
    }

    public Collection<GComponentSynonym> getSynonyms() {
        return getRelatedGNodeCollection(
                GComponentSynonym.class,
                GComponentSynonymImpl.class,
                GLinkType.COMPONENT_TO_SYNONYM,
                Direction.OUTGOING);
    }

    public boolean isSetType() {
        return (getType() != null);
    }

    public GComponentLinks getGComponentLinks() {
        return getRelatedGNode(
                GComponentLinks.class,
                GComponentLinksImpl.class,
                GLinkType.COMPONENT_TO_LINKS,
                Direction.OUTGOING);
    }

    public Collection<GComponentLink> getLinks() {
        return getGComponentLinks().getLinks();
    }

    public boolean isSetName() {
        return (getName() != null);
    }

}
