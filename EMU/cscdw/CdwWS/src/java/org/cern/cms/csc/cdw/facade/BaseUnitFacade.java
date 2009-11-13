/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.BaseUnit;

/**
 *
 * @author valdo
 */
@Stateless
public class BaseUnitFacade implements BaseUnitFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(BaseUnit baseUnit) {
        em.persist(baseUnit);
    }

    public void edit(BaseUnit baseUnit) {
        em.merge(baseUnit);
    }

    public void remove(BaseUnit baseUnit) {
        em.remove(em.merge(baseUnit));
    }

    public BaseUnit find(Object id) {
        return em.find(BaseUnit.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<BaseUnit> findAll() {
        return em.createQuery("select object(o) from BaseUnit as o").getResultList();
    }

}
