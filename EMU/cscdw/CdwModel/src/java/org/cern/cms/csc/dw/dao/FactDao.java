/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import java.io.Serializable;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.model.fact.FactCollection;

/**
 *
 * @author valdo
 */
@Stateless
public class FactDao implements FactDaoLocal, Serializable {

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    public void saveFactCollection(final FactCollection factCollection) throws Exception {
        em.persist(factCollection);
    }
 
}
