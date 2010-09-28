/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;

/**
 *
 * @author valdo
 */
@Local
public interface OntologyDaoLocal extends GOntologyDaoLocal {

    ComponentClass getComponentClassById(Long id) throws ComponentClassNotFoundException;
    Component getComponentById(Long id) throws ComponentNotFoundException;
    Component getComponentByName(String name) throws ComponentNotFoundException;
    Component getComponent(GComponent gcomp) throws ComponentNotFoundException;
    ComponentLinkClass getComponentLinkClassById(Long id) throws ComponentLinkClassNotFoundException;
    
}
