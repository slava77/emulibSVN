/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;

/**
 *
 * @author Evka
 */
@Local
public interface RuleEngineDaoLocal {

    List<RuleSet> getAllRuleSets();
    List<RuleSet> getAllEnabledRuleSets();

    List<Rule> getRules(RuleSet ruleSet);
    List<Rule> getEnabledRules(RuleSet ruleSet);
    void saveRule(Rule rule) throws Exception;
    void cloneAndSaveRule(Rule rule);

    List<Conclusion> getAllConclusions();
    List<Conclusion> getAllOpenConclusions();
    List<Conclusion> getAllOpenConclusions(boolean loadLazyRelations);
    List<Conclusion> getAllOpenTopConclusions();
    List<Conclusion> getOpenTopConclusions(boolean acknowledged);
    List<Conclusion> getAllClosedTopConclusions();

    List<ConclusionType> getConclusionTypes(RuleSet ruleSet);

    EntityDaoLocal getEntityDao();
    
}
