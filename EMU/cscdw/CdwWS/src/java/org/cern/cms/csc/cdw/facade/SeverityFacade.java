/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.Severity;

/**
 *
 * @author valdo
 */
@Stateless
public class SeverityFacade implements SeverityFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(Severity severity) {
        em.persist(severity);
    }

    public void edit(Severity severity) {
        em.merge(severity);
    }

    public void remove(Severity severity) {
        em.remove(em.merge(severity));
    }

    public Severity find(Object id) {
        return em.find(Severity.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<Severity> findAll() {
        return em.createQuery("select object(o) from Severity as o").getResultList();
    }

}
