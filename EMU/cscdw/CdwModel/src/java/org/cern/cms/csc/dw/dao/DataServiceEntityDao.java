/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.model.dse.DataServiceEntity;

/**
 *
 * @author Evka
 */
@Stateless
public class DataServiceEntityDao implements DataServiceEntityDaoLocal {

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;
    
    public void saveDataServiceEntity(DataServiceEntity dse) throws Exception {
        em.persist(dse);
    }
 
}
