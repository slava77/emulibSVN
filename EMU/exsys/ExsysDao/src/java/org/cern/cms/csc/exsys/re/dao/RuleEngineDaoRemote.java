/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.Remote;
import org.cern.cms.csc.exsys.re.model.RuleSet;

/**
 *
 * @author Evka
 */
@Remote
public interface RuleEngineDaoRemote {

    List<RuleSet> getAllRuleSets();
    void cloneAndSaveRuleSet(RuleSet ruleSet);

}
