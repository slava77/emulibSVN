/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 *
 * @author evka
 */
@Stateless
public class ConclusionDao implements ConclusionDaoLocal {
    
    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    @EJB
    private EntityDaoLocal entityDao;

    public ConclusionDao() {
    }

    @SuppressWarnings("unchecked")
    public List<Conclusion> getAllConclusions() {
        List<Conclusion> conclusions = em.createQuery("select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                                                      "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    public List<Conclusion> getAllOpenConclusions() {
        return getAllOpenConclusions(false);
    }

    @SuppressWarnings("unchecked")
    public List<Conclusion> getAllOpenConclusions(boolean getAllOpenConclusions) {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.isClosed = false " +
                "order by c.timestampItem desc").getResultList();
        if (getAllOpenConclusions) {
            for (Conclusion concl: conclusions) {
                concl.getTriggers().iterator();
                concl.getParents().iterator();
            }
        }
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    public List<Conclusion> getAllOpenTopConclusions() {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.isClosed = false and " +
                "c.parents is empty " +
                "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    public List<ConclusionType> getAllConclusionTypes() {
        List<ConclusionType> conclusionTypes = em.createQuery("select ct from org.cern.cms.csc.exsys.re.model.ConclusionType as ct " +
                                                      "order by ct.name desc").getResultList();
        return conclusionTypes;
    }

    @SuppressWarnings("unchecked")
    public List<Conclusion> getOpenTopConclusions(boolean acknowledged) {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.isClosed = false and " +
                "c.parents is empty and " +
                "c.isAcknowledged = :ack " +
                "order by c.timestampItem desc")
                .setParameter("ack", acknowledged)
                .getResultList();
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    public List<Conclusion> getAllClosedTopConclusions() {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.isClosed = true and " +
                "c.parents is empty " +
                "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    public EntityDaoLocal getEntityDao() {
        return entityDao;
    }

}
