package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.Collection;
import java.util.Map;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.neo4j.graphdb.Direction;

public interface GComponent extends GNode {

    public enum DataPropertyType implements GNode.PropertyType {

        DCS_ID("_DCSId");

        private final String owlName;

        DataPropertyType(String argOwlName) {
            owlName = argOwlName;
        }

        @Override
        public String propertyName() {
            return "data." + name();
        }

        public String getPropertyOwlName() {
            return owlName;
        }

        public static DataPropertyType fromOwlName(String argOwlName) {
            for (DataPropertyType enumItem: DataPropertyType.values()) {
                if (enumItem.owlName.equals(argOwlName)) {
                    return enumItem;
                }
            }
            return null;
        }

        public static DataPropertyType valueOfSilent(String name) {
            for (DataPropertyType enumItem: DataPropertyType.values()) {
                if (enumItem.name().equals(name)) {
                    return enumItem;
                }
            }
            return null;
        }
    };

    void setName(String name);
    String getName();
    boolean isSetName();

    void setType(GComponentClass type);
    GComponentClass getType();
    boolean isSetType();
    Collection<GComponentClass> getTypes();

    void setDescription(String description);
    String getDescription();

    void addSynonym(GComponentSynonym synonym);
    Collection<GComponentSynonym> getSynonyms();

    GComponentLinks getGComponentLinks();
    Collection<GComponentLink> getLinks();

    void setDataProperty(DataPropertyType type, Object value);
    Object getDataProperty(DataPropertyType type);
    boolean isSetDataProperty(DataPropertyType type);
    Map<DataPropertyType, Object> getAllDataProperties();

    Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType);
    Collection<GComponent> getRelatedGComponents(ComponentLinkClassType linkType, Direction dir);

    boolean hasRelatedGComponents(ComponentLinkClassType linkType);
    boolean hasRelatedGComponents(ComponentLinkClassType linkType, Direction dir);

    /**
     * Find all particularly related components recursively.
     * @param linkType Link type to look for.
     * @return
     */
    Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType);

    /**
     * Find all related components of particular type.
     * @param linkType Link type to look for.
     * @param type Result type
     * @return
     */
    Collection<GComponent> findRelatedGComponents(ComponentLinkClassType linkType, GComponentClass type);

    /**
     * Checks if the given component is related to this component via the given link type (recursive).
     * @param linkType link type to check.
     * @param gComponent component to check.
     * @return
     */
    boolean isRelatedToRecusively(ComponentLinkClassType linkType, GComponent gComponent);

}
