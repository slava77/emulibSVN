/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.cdw.model.Source;

/**
 *
 * @author valdo
 */
@Stateless
public class SourceFacade implements SourceFacadeLocal {
    @PersistenceContext
    private EntityManager em;

    public void create(Source source) {
        em.persist(source);
    }

    public void edit(Source source) {
        em.merge(source);
    }

    public void remove(Source source) {
        em.remove(em.merge(source));
    }

    public Source find(Object id) {
        return em.find(Source.class, id);
    }

    @SuppressWarnings("unchecked")
    public List<Source> findAll() {
        return em.createQuery("select object(o) from Source as o").getResultList();
    }

}
