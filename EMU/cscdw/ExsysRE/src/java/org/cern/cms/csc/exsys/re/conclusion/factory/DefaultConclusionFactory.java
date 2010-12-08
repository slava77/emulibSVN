/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.factory;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheService;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionSourceRelation;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
public class DefaultConclusionFactory extends ConclusionFactory {

    private static Logger logger = Logger.getLogger(ConclusionFactory.class.getName());

    private RuleEngineDaoLocal reDao;
    private ConclusionCacheService conclusionCacheService;

    public DefaultConclusionFactory(RuleEngineManagerLocal reManager, Rule rule, ConclusionCacheService conclusionCacheService) {
        super(reManager, rule);
        this.reDao = reManager.getRuleEngineDao();
        this.conclusionCacheService = conclusionCacheService;
    }

    @Override
    protected Conclusion processConclusion(Conclusion conclusion) {
        try {
            // check if it already exists in cache
            Conclusion existingConclusion = getConclusionCacheService().checkCache(conclusion);
            if (existingConclusion == null) {
                logger.log(Level.INFO, "Default conclusion factory: Got new conclusion: {0}", conclusion.getTitle());

                logger.info("Saving new conclusion:");
                logger.info(conclusion.debugPrint());
                reDao.getEntityDao().getPersistDao().persist(conclusion);
                getConclusionCacheService().addToCache(conclusion);
                return conclusion;
            } else {
                logger.log(Level.FINE, "Default conclusion factory: Got a duplicate conclusion (updating existing one): {0}",
                        conclusion.getTitle());

                //existingConclusion = (Conclusion) reDao.getEntityDao().refreshEntity(existingConclusion);
                updateExistingConclusion(existingConclusion, conclusion);
                logger.info("Saving existing conclusion:");
                logger.info(existingConclusion.debugPrint());
                existingConclusion = (Conclusion) reDao.getEntityDao().getPersistDao().merge(existingConclusion);
                getConclusionCacheService().addToCache(existingConclusion); // update the conclusion in the cache
                return existingConclusion;
            }
        } catch (Exception pex) {
            logger.log(Level.SEVERE, "Exception while saving a conclusion", pex);
            return null;
        }
    }

    protected void updateExistingConclusion(Conclusion existingConclusion, Conclusion conclusion) {
        existingConclusion.setHitCount(existingConclusion.getHitCount().add(BigInteger.ONE));
        existingConclusion.setLastHitTimeItem(conclusion.getTimestampItem());
        existingConclusion.setTitle(conclusion.getTitle()); // update with more up to date title
        existingConclusion.setDescription(conclusion.getDescription()); // update with more up to date description

        // merge source relations
        Set<Long> existingChildFactIds = new HashSet<Long>();
        Set<Long> existingChildConclusionIds = new HashSet<Long>();

        for (ConclusionSourceRelation rel: existingConclusion.getChildren()) {
            if (rel.getChildFact() != null) {
                existingChildFactIds.add(rel.getChildFact().getid());
            } else if (rel.getChildConclusion() != null) {
                existingChildConclusionIds.add(rel.getChildConclusion().getid());
            }
        }

        List<ConclusionSourceRelation> relationsToCheck = new ArrayList<ConclusionSourceRelation>();
        relationsToCheck.addAll(conclusion.getChildren());
        conclusion = null;
        List<ConclusionSourceRelation> relationsToAdd = new ArrayList<ConclusionSourceRelation>();
        for (ConclusionSourceRelation rel: relationsToCheck) {
            if (rel.getChildFact() != null) {
                if (!existingChildFactIds.contains(rel.getChildFact().getid())) {
                    rel.setParent(existingConclusion);
                    relationsToAdd.add(rel);
                }
            } else if (rel.getChildConclusion() != null) {
                if (!existingChildConclusionIds.contains(rel.getChildConclusion().getid())) {
                    rel.setParent(existingConclusion);
                    relationsToAdd.add(rel);
                }
            }
        }

        existingConclusion.getChildren().addAll(relationsToAdd);
    }

    protected ConclusionCacheService getConclusionCacheService() {
        return conclusionCacheService;
    }

}
