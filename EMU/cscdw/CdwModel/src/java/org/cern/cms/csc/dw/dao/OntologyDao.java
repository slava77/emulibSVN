package org.cern.cms.csc.dw.dao;

import java.io.Serializable;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.exception.OntologyException.OntologySource;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;

@Stateless
@SuppressWarnings("unchecked")
public class OntologyDao extends GOntologyDao implements OntologyDaoLocal, Serializable {
    
    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    public Component getComponentById(Long id) throws ComponentNotFoundException {
        Component c = (Component) em.find(Component.class, id);        
        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.RELATIONAL_DATABASE, id);
        }
        return c;
    }

    public ComponentClass getComponentClassById(Long id) throws ComponentClassNotFoundException {
        ComponentClass cc = em.find(ComponentClass.class, id);
        if (cc == null) {
            throw new ComponentClassNotFoundException(OntologySource.RELATIONAL_DATABASE, id);
        }
        cc.getParents().iterator();
        return cc;
    }

    public ComponentLinkClass getComponentLinkClassById(Long id) throws ComponentLinkClassNotFoundException {
        ComponentLinkClass cc = em.find(ComponentLinkClass.class, id);
        if (cc == null) {
            throw new ComponentLinkClassNotFoundException(OntologySource.RELATIONAL_DATABASE, id);
        }
        cc.getParent();
        return cc;
    }

    public Component getComponent(GComponent gcomp) throws ComponentNotFoundException {
        return getComponentByName(gcomp.getName());
    }

    public Component getComponentByName(String name) throws ComponentNotFoundException {
        Component c = (Component) em.createQuery("select c from Component c where name = ?")
                                    .setParameter(0, name)
                                    .getSingleResult();
        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.RELATIONAL_DATABASE, name);
        }
        return c;
    }

}
