package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.Map;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.ReturnableEvaluator;
import org.neo4j.graphdb.StopEvaluator;

public class GComponentImpl extends GNodeImpl implements GComponent {

    public GComponentImpl(GServices gservices, Node node) {
        super(gservices, node);
    }

    @Override
    public void setName(String name) {
        setProperty(InternalPropertyType.NAME, name, true);
    }

    @Override
    public String getName() {
        return (String) getProperty(InternalPropertyType.NAME);
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
    public void setType(GComponentClass type) {
        setRelationship(type, GLinkType.COMPONENT_TO_CLASS);
    }

    @Override
    public GComponentClass getType() {
        return getRelatedGNode(
                GComponentClass.class,
                GLinkType.COMPONENT_TO_CLASS,
                Direction.OUTGOING);
    }

    @Override
    public void addSynonym(GComponentSynonym synonym) {
        if (!getSynonyms().contains(synonym) && synonym.getComponent() == null) {
            setRelationship(synonym, GLinkType.COMPONENT_TO_SYNONYM);
        }
    }

    @Override
    public Collection<GComponentSynonym> getSynonyms() {
        return getRelatedGNodeCollection(
                GComponentSynonym.class,
                GLinkType.COMPONENT_TO_SYNONYM,
                Direction.OUTGOING);
    }

    @Override
    public boolean isSetType() {
        return (getType() != null);
    }

    @Override
    public Object getDataProperty(DataPropertyType type) {
        return getProperty(type);
    }

    @Override
    public boolean isSetDataProperty(DataPropertyType type) {
        return (getDataProperty(type) != null);
    }

    @Override
    public void setDataProperty(DataPropertyType type, Object value) {
        setProperty(type, value, true);
    }

    @Override
    public Map<DataPropertyType, Object> getAllDataProperties() {
        Map<DataPropertyType, Object> ret = new HashMap<DataPropertyType, Object>();
        Iterator<String> propKeyIt = getAllPropertyKeys().iterator();
        for (;propKeyIt.hasNext();) {
            String propKey = propKeyIt.next();
            String[] propKeyParts = propKey.split("\\.");
            DataPropertyType propType = DataPropertyType.valueOfSilent(propKeyParts[propKeyParts.length - 1]);
            if (propType != null) {
                ret.put(propType, getDataProperty(propType));
            }
        }
        return ret;
    }

    @Override
    public GComponentLinks getGComponentLinks() {
        return getRelatedGNode(
                GComponentLinks.class,
                GLinkType.COMPONENT_TO_LINKS,
                Direction.OUTGOING);
    }

    @Override
    public Collection<GComponentLink> getLinks() {
        return getGComponentLinks().getLinks();
    }

    @Override
    public boolean isSetName() {
        return (getName() != null);
    }

    @Override
    public Collection<GComponentClass> getTypes() {
        return getType().getParentsRecursive(true);
    }

    @Override
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

    @Override
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

    @Override
    public Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType) {
        return getRelatedGComponents(linkType, Direction.OUTGOING);
    }

    @Override
    public boolean hasRelatedGComponents(ComponentLinkClassType linkType) {
        return hasRelatedGComponents(linkType, Direction.OUTGOING);
    }

    @Override
    public Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType, GComponentClass type) {
        Collection<GComponent> ret = new LinkedList<GComponent>();
        for (GComponent gc: findRelatedGComponents(linkType)) {
            if (gc.getType().equals(type)) {
                ret.add(gc);
            }
        }
        return ret;
    }

    @Override
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

    @Override
    public boolean isRelatedToRecusively(ComponentLinkClassType linkType, GComponent gComponent) {
        return findRelatedGComponents(linkType).contains(gComponent);
    }

}
