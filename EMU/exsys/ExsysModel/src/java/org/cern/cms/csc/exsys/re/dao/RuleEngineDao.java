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
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;

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

    @Override
    public List<RuleSet> getAllRuleSets() {
        List<RuleSet> ruleSets = em.createQuery("select rs from org.cern.cms.csc.exsys.re.model.RuleSet as rs " +
                                                "order by rs.name, rs.version desc", RuleSet.class)
                                                .getResultList();
        return ruleSets;
    }

    @Override
    public List<RuleSet> getAllEnabledRuleSets() {
        List<RuleSet> ruleSets = em.createQuery("select rs from org.cern.cms.csc.exsys.re.model.RuleSet as rs " +
                                                "where rs.enabled = true " +
                                                "order by rs.name, rs.version desc", RuleSet.class)
                                                .getResultList();
        return ruleSets;
    }

    @Override
    public List<Rule> getEnabledRules(RuleSet ruleSet) {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r " +
                                          "where r.conclusionType.ruleSet = :ruleSet and r.enabled = true " +
                                          "order by r.name", Rule.class)
                                          .setParameter("ruleSet", ruleSet)
                                          .getResultList();
        return rules;
    }

    /**
     * Get all rules (both active and inactive)
     * @return all rules (both active and inactive)
     */
    @SuppressWarnings("unchecked")
    @Override
    public List<Rule> getRules(RuleSet ruleSet) {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r " +
                                          "where r.conclusionType.ruleSet = :ruleSet " +
                                          "order by r.name", Rule.class)
                                          .setParameter("ruleSet", ruleSet)
                                          .getResultList();
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
                "where c.closed = false " +
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
                "where c.closed = false and " +
                "c.parents is empty " +
                "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @SuppressWarnings("unchecked")
    @Override
    public List<ConclusionType> getConclusionTypes(RuleSet ruleSet) {
        List<ConclusionType> conclusionTypes = em.createQuery("select ct from org.cern.cms.csc.exsys.re.model.ConclusionType as ct " +
                                                      "where ct.ruleSet = :ruleSet " +
                                                      "order by ct.name desc", ConclusionType.class)
                                                      .setParameter("ruleSet", ruleSet)
                                                      .getResultList();
        return conclusionTypes;
    }

    @SuppressWarnings("unchecked")
    @Override
    public List<Conclusion> getOpenTopConclusions(boolean acknowledged) {
        List<Conclusion> conclusions = em.createQuery(
                "select c from org.cern.cms.csc.exsys.re.model.Conclusion as c " +
                "where c.closed = false and " +
                "c.parents is empty and " +
                "c.acknowledged = :ack " +
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
                "where c.closed = true and " +
                "c.parents is empty " +
                "order by c.timestampItem desc").getResultList();
        return conclusions;
    }

    @Override
    public void saveRule(Rule rule) throws Exception {
//        if (rule.getid() == null) {
//
//            List list  = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r " +
//                                                  "where r.name = :name and r.conclusionType = :conclType")
//                                                  .setParameter("name", rule.getName())
//                                                  .setParameter("conclType", rule.getConclusionType())
//                                                  .getResultList();
//            if (!list.isEmpty()) {
//                throw new PersistException("Rule with name " + rule.getName() + " and conclusion type already exists in database!");
//            }
//
//        } else {
//
//            List conclusionTriggers = em.createQuery("select ct from org.cern.cms.csc.exsys.re.model.ConclusionTrigger as ct " +
//                                                  "where ct.rule = :rule")
//                                                  .setParameter("rule", rule)
//                                                  .setMaxResults(1)
//                                                  .getResultList();
//            if (!conclusionTriggers.isEmpty()) {
//                throw new UnsupportedOperationException("Ruleset versioning is not yet supported - cannot update rule which has already produced conclusion triggers");
//            }
//
//        }

        entityDao.merge(rule);

    }

}
