/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.exception.DaoException;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
@Local
public interface RuleEngineDaoLocal {

    List<Rule> getAllRules();
    List<Rule> getRulesByName(String name);
    List<Rule> getRulesByConclusionType(ConclusionType conclusionType);
    Rule getActiveRule(String name) throws DaoException;
    EntityDaoLocal getEntityDao();

}
