/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.Domain;

/**
 *
 * @author valdo
 */
@Stateless
public class DomainFacade implements DomainFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(Domain domain) {
        em.persist(domain);
    }

    public void edit(Domain domain) {
        em.merge(domain);
    }

    public void remove(Domain domain) {
        em.remove(em.merge(domain));
    }

    public Domain find(Object id) {
        return em.find(Domain.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<Domain> findAll() {
        return em.createQuery("select object(o) from Domain as o").getResultList();
    }

}
