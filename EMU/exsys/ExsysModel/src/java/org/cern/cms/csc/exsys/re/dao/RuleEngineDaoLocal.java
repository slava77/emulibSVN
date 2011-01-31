/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.Local;
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
@Local
public interface RuleEngineDaoLocal {

    List<Rule> getRules();
    void saveRule(Rule rule) throws PersistException, OnSaveProcessingException;

    List<Conclusion> getAllConclusions();
    List<Conclusion> getAllOpenConclusions();
    List<Conclusion> getAllOpenConclusions(boolean loadLazyRelations);
    List<Conclusion> getAllOpenTopConclusions();
    List<Conclusion> getOpenTopConclusions(boolean acknowledged);
    List<Conclusion> getAllClosedTopConclusions();

    List<ConclusionType> getAllConclusionTypes();

    EntityDaoLocal getEntityDao();

}
