/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.Collections;
import java.util.List;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;

/**
 *
 * @author Evka
 */
@Stateless
public class RuleEngineDao implements RuleEngineDaoLocal, RuleEngineDaoRemote {

    private static final Logger logger = ExsysLogger.getLogger(RuleEngineDao.class);

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
                                                      "order by c.timestamp desc").getResultList();
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
                "order by c.timestamp desc").getResultList();
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
                "order by c.timestamp desc").getResultList();
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
                "order by c.timestamp desc")
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
                "order by c.timestamp desc").getResultList();
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

    @Override
    public void cloneAndSaveRuleSet(RuleSet ruleSet) {
        logger.info("RuleEngineDao: got this ruleset for cloning and saving: " + ruleSet);
        ruleSet.setid(null);
        for (ConclusionType ct: ruleSet.getConclusionTypes()) {
            logger.info("RuleEngineDao: processing this conclusion type: " + ct);
            ct.setid(null);
            //TODO support actions in ruleset cloning
            // throw away actions.. it's a many-to-many relationship - not easy to handle
            ct.setActions(Collections.EMPTY_LIST);
            for (Rule rule: ct.getRules()) {
                logger.info("RuleEngineDao: processing this rule: " + rule);
                rule.setid(null);
                rule.getComponentFinder().setid(null);

                // get the component class from production db
                ComponentClass compClass = rule.getComponentFinder().getComponentClass();
                compClass = em.createQuery("select cc from org.cern.cms.csc.dw.model.ontology.ComponentClass as cc where cc.type = :type", ComponentClass.class)
                            .setParameter("type", compClass.getType())
                            .getSingleResult();
                rule.getComponentFinder().setComponentClass(compClass);
            }
        }
        logger.info("RuleEngineDao: persisting this ruleset: " + ruleSet);

        em.persist(ruleSet);
        for (ConclusionType ct: ruleSet.getConclusionTypes()) {
            for (Rule rule: ct.getRules()) {
                em.persist(rule);
            }
        }
    }

}
