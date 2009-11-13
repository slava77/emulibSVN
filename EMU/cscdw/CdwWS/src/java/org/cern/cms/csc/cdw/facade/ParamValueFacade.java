/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.ParamValue;

/**
 *
 * @author valdo
 */
@Stateless
public class ParamValueFacade implements ParamValueFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(ParamValue paramValue) {
        em.persist(paramValue);
    }

    public void edit(ParamValue paramValue) {
        em.merge(paramValue);
    }

    public void remove(ParamValue paramValue) {
        em.remove(em.merge(paramValue));
    }

    public ParamValue find(Object id) {
        return em.find(ParamValue.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<ParamValue> findAll() {
        return em.createQuery("select object(o) from ParamValue as o").getResultList();
    }

}
