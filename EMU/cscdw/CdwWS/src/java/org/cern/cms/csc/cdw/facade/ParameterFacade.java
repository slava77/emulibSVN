/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.Parameter;

/**
 *
 * @author valdo
 */
@Stateless
public class ParameterFacade implements ParameterFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(Parameter parameter) {
        em.persist(parameter);
    }

    public void edit(Parameter parameter) {
        em.merge(parameter);
    }

    public void remove(Parameter parameter) {
        em.remove(em.merge(parameter));
    }

    public Parameter find(Object id) {
        return em.find(Parameter.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<Parameter> findAll() {
        return em.createQuery("select object(o) from Parameter as o").getResultList();
    }

}
