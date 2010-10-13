/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
@Stateless
public class RuleEngineDao implements RuleEngineDaoLocal {

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    @SuppressWarnings("unchecked")
    public List<Rule> getAllRules() {
        List<Rule> rules = em.createQuery("select r from org.cern.cms.csc.exsys.re.model.Rule as r").getResultList();
        return rules;
    }

    public Rule getRule(String name) {
        return null;
    }
}
