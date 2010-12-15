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
import org.cern.cms.csc.exsys.exception.DaoException;
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
    public List<Rule> getAllRules() {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r order by r.name, r.version desc").getResultList();
        return rules;
    }

    /**
     * Get active rule with the provided name
     * @param name name of the rule
     * @return active rule with the given name
     * @throws DaoException if more than one active rule with the given name is found, or if no active rule with the given name was found
     */
    @SuppressWarnings("unchecked")
    public Rule getActiveRule(String name) throws DaoException {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r where r.name = :name and r.enabled = true")
                .setParameter("name", name)
                .getResultList();
        if (rules.size() > 1) {
            throw new DaoException("Found more than one active rule with name = " + name);
        }
        if (rules.size() == 0) {
            throw new DaoException("No active rule with name \"" + name + "\" was found");
        }
        return rules.get(0);
    }

    @SuppressWarnings("unchecked")
    public List<Rule> getRulesByName(String name) {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r " +
                                          "where r.name = :name " +
                                          "order by r.version desc")
                                          .setParameter("name", name)
                                          .getResultList();
        return rules;
    }

    @SuppressWarnings("unchecked")
    public List<Rule> getRulesByConclusionType(ConclusionType conclusionType) {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r " +
                                          "where r.conclusionType = :conclType " +
                                          "order by r.name, r.version desc")
                                          .setParameter("conclType", conclusionType)
                                          .getResultList();
        return rules;
    }

    public EntityDaoLocal getEntityDao() {
        return entityDao;
    }
    
}
