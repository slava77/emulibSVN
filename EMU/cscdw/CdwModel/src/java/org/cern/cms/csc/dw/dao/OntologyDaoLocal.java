/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
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

    ComponentClass getComponentClassById(Long id);
    List<ComponentClass> getComponentClasses(ComponentClass parent);

    Component getComponentById(Long id, boolean eager) throws ComponentNotFoundException;
    Component getComponentByName(String id) throws ComponentNotFoundException;
    Component getComponentByName(String id, boolean eager) throws ComponentNotFoundException;
    List<Component> getComponents(ComponentClass componentClass);
    
    List<ComponentLinkClass> getComponentLinkClasses(ComponentLinkClass parent);
    ComponentLinkClass getComponentLinkClassByName(String id);
    List<ComponentLink> getComponentLinks(ComponentLinkClass componentLinkClass);
    ComponentLink getComponentLinkByName(String id);
    
}
