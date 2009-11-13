/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.Unit;

/**
 *
 * @author valdo
 */
@Stateless
public class UnitFacade implements UnitFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(Unit unit) {
        em.persist(unit);
    }

    public void edit(Unit unit) {
        em.merge(unit);
    }

    public void remove(Unit unit) {
        em.remove(em.merge(unit));
    }

    public Unit find(Object id) {
        return em.find(Unit.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<Unit> findAll() {
        return em.createQuery("select object(o) from Unit as o").getResultList();
    }

}
