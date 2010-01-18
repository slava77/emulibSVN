/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLink;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;

/**
 *
 * @author valdo
 */
@Local
public interface OntologyDaoLocal {

    ComponentClass getComponentClassById(String id);
    List<ComponentClass> getComponentClasses(ComponentClass parent);

    Component getComponentById(String id);
    Component getComponentById(String id, boolean eager);
    List<Component> getComponents(ComponentClass componentClass);
    
    List<ComponentLinkClass> getComponentLinkClasses(ComponentLinkClass parent);
    ComponentLinkClass getComponentLinkClassById(String id);
    List<ComponentLink> getComponentLinks(ComponentLinkClass componentLinkClass);
    ComponentLink getComponentLinkById(String id);
    
}
