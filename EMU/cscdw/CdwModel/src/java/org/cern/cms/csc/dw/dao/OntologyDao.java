/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.io.Serializable;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLink;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.ComponentSynonym;

/**
 *
 * @author valdo
 */
@Stateless
public class OntologyDao implements OntologyDaoLocal, Serializable {
    
    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    public Component getComponentByName(String name, boolean eager) throws ComponentNotFoundException {
        Component c = (Component) em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.Component as c where name = ?")
                .setParameter("name", name)
                .setHint("org.hibernate.cacheable", new Boolean(true))
                .getSingleResult();

        if (c == null) {
            ComponentSynonym cs = em.find(ComponentSynonym.class, name);
            if (cs != null) {
                c = (Component) em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.Component as c where ? = any elements(c.synonyms)")
                        .setParameter(1, cs)
                        .setHint("org.hibernate.cacheable", new Boolean(true))
                        .getSingleResult();
            }
        }

        // Fetch collections if needed (if eager)
        if (c == null) {
            throw new ComponentNotFoundException(name);
        } else {
            if (eager) {
                c.getLinks().iterator();
                c.getSynonyms().iterator();
            }
        }

        return c;
    }

    /**
     * Finds and returns a components with appropriate id or (if not found) synonym. Otherwise returns null
     * @param id
     * @return
     */
    public Component getComponentByName(String name) throws ComponentNotFoundException {
        return getComponentByName(name, false);
    }

    public ComponentClass getComponentClassByName(String name) {
        ComponentClass cc = em.find(ComponentClass.class, name);
        if (cc != null) {
            cc.getParents().iterator();
        }
        return cc;
    }

    @SuppressWarnings("unchecked")
    public List<ComponentClass> getComponentClasses(ComponentClass parent) {
        if (parent == null) {
            return em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.ComponentClass as c where size(c.parents) = 0 order by c.name")
                    .setHint("org.hibernate.cacheable", new Boolean(true))
                    .getResultList();
        } else {
            return em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.ComponentClass as c where ? = any elements(c.parents) order by c.name")
                    .setParameter(1, parent)
                    .setHint("org.hibernate.cacheable", new Boolean(true))
                    .getResultList();
        }
    }

    @SuppressWarnings("unchecked")
    public List<Component> getComponents(ComponentClass componentClass) {
        return em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.Component as c where c.componentClass = ? order by c.name")
                .setParameter(1, componentClass)
                .setHint("org.hibernate.cacheable", new Boolean(true))
                .getResultList();
    }

    @SuppressWarnings("unchecked")
    public List<ComponentLinkClass> getComponentLinkClasses(ComponentLinkClass parent) {
        if (parent == null) {
            return em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.ComponentLinkClass as c where c.parent is null order by c.name")
                    .setHint("org.hibernate.cacheable", new Boolean(true))
                    .getResultList();
        } else {
            return em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.ComponentLinkClass as c where c.parent = ? order by c.name")
                    .setParameter(1, parent)
                    .setHint("org.hibernate.cacheable", new Boolean(true))
                    .getResultList();
        }
    }

    public ComponentLinkClass getComponentLinkClassByName(String name) {
        ComponentLinkClass cc = em.find(ComponentLinkClass.class, name);
        if (cc != null) {
            cc.getParent();
        }
        return cc;
    }

    @SuppressWarnings("unchecked")
    public List<ComponentLink> getComponentLinks(ComponentLinkClass componentLinkClass) {
        return em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.ComponentLink as c where c.componentLinkClass = ? order by c.name")
                .setParameter(1, componentLinkClass)
                .setHint("org.hibernate.cacheable", new Boolean(true))
                .getResultList();
    }

    public ComponentLink getComponentLinkByName(String name) {
        ComponentLink c = em.find(ComponentLink.class, name);

        if (c != null) {
            c.getComponent();
            c.getComponentLinkClass();
        }

        return c;
    }

}
