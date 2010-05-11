/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLink;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.ComponentSynonym;

/**
 * Ontology Component Base class
 * @author valdo
 */
public abstract class ComponentBase extends EntityBase {

    /**
     * Component Class
     * @return
     */
    public abstract ComponentClass getComponentClass();

    /**
     * Component Id
     * @return
     */
    public abstract Long getId();

    /**
     * Links to other components
     * @return
     */
    public abstract List<ComponentLink> getLinks();

    /**
     * Synonyms
     * @return
     */
    public abstract List<ComponentSynonym> getSynonyms();

    /**
     * Component class is not null ?
     * @return
     */
    public abstract boolean isSetComponentClass();

    /**
     * Id is not null ?
     * @return
     */
    public boolean isSetId() {
        return (getId() != null);
    }

    /**
     * Links is not null and exists at least one item?
     * @return
     */
    public abstract boolean isSetLinks();

    /**
     * Synonyms is not null and exists at least one item?
     * @return
     */
    public abstract boolean isSetSynonyms();


    /**
     * Does this component has a direct link of specified class to another specified component
     * @param component
     * @param linkClass
     * @return
     */
    public boolean isDirectLink(ComponentBase component, ComponentLinkClass linkClass) {
        for (ComponentLink cl: getLinks()) {
            if (cl.getComponent().getId().equals(component.getId()) && cl.getComponentLinkClass().equals(linkClass)) {
                return true;
            }
        }
        return false;
    }

    /**
     * A list of unique link classes that this component has to other components
     * @return
     */
    public Set<ComponentLinkClass> getAvailableLinkClasses() {
        Set<ComponentLinkClass> links = new HashSet<ComponentLinkClass>();
        for (ComponentLink cl: getLinks()) {
            if (!links.contains(cl.getComponentLinkClass())) {
                links.add(cl.getComponentLinkClass());
            }
        }
        return links;
    }

}
