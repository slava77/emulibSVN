/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.math.BigInteger;
import java.util.List;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.criteria.CriteriaBuilder;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
@Stateless
public class RuleEngineDao implements RuleEngineDaoLocal {

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    @EJB
    private EntityDaoLocal entityDao;

    /**
     * Get all rules (both active and inactive)
     * @return all rules (both active and inactive)
     */
    @SuppressWarnings("unchecked")
    @Override
    public List<Rule> getRules() {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r order by r.name, r.version desc").getResultList();
        return rules;
    }

    @Override
    public EntityDaoLocal getEntityDao() {
        return entityDao;
    }
 
    @SuppressWarnings("unchecked")
    @Override
    public List<Conclusion> getAllConclusions() {
        List<Conclusion> conclusions = em.createQuery("select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                                                      "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    @Override
    public List<Conclusion> getAllOpenConclusions() {
        return getAllOpenConclusions(false);
    }

    @SuppressWarnings("unchecked")
    @Override
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
    @Override
    public List<Conclusion> getAllOpenTopConclusions() {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.isClosed = false and " +
                "c.parents is empty " +
                "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    @Override
    public List<ConclusionType> getAllConclusionTypes() {
        List<ConclusionType> conclusionTypes = em.createQuery("select ct from org.cern.cms.csc.exsys.re.model.ConclusionType as ct " +
                                                      "order by ct.name desc").getResultList();
        return conclusionTypes;
    }

    @SuppressWarnings("unchecked")
    @Override
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
    @Override
    public List<Conclusion> getAllClosedTopConclusions() {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.isClosed = true and " +
                "c.parents is empty " +
                "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @Override
    public void saveRule(Rule rule) throws PersistException, OnSaveProcessingException {
        if (rule.getid() == null) {

            List list  = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r " +
                                                  "where r.name = :name")
                                                  .setParameter("name", rule.getName())
                                                  .getResultList();
            if (!list.isEmpty()) {
                throw new PersistException("Rule with name " + rule.getName() + " already exists in database!");
            }
            rule.setVersion(BigInteger.ONE);

        } else {

            List conclusionTriggers = em.createQuery("select ct from org.cern.cms.csc.exsys.re.model.ConclusionTrigger as ct " +
                                                  "where ct.rule = :rule")
                                                  .setParameter("rule", rule)
                                                  .setMaxResults(1)
                                                  .getResultList();
            if (!conclusionTriggers.isEmpty()) {
                rule.setVersion(rule.getVersion().add(BigInteger.ONE));
                rule.setid(null);
            }

        }

        entityDao.merge(rule);

    }

}
