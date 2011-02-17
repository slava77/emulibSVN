/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.ejb.EJB;
import javax.ejb.Singleton;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionTrigger;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 *
 * @author evka
 */
@Singleton
public class ConclusionCacheService implements ConclusionCacheServiceLocal {

    private static final Logger logger = SimpleLogger.getLogger(ConclusionCacheService.class);

    @EJB
    private RuleEngineDaoLocal reDao;
    private Map<ComparableConclusionWrapper, Conclusion> conclusionCache;

    public ConclusionCacheService() {
    }

    /**
     * Checks if the given conclusion exists in the cache of open conclusions.
     * If it does, then returns the original conclusion that was found in the cache.
     * If it doesn't, returns null.
     */
    @Override
    public Conclusion checkCache(Conclusion conclusion) {
        if (conclusionCache == null) { // initialize the cache
            conclusionCache = Collections.synchronizedMap(new HashMap<ComparableConclusionWrapper, Conclusion>());
            List<Conclusion> conclusions = reDao.getAllOpenConclusions();
            for (Conclusion concl: conclusions) {
                conclusionCache.put(new ComparableConclusionWrapper(concl), concl);
            }
        }

        ComparableConclusionWrapper conclWrapper = new ComparableConclusionWrapper(conclusion);
        if (conclusionCache.containsKey(conclWrapper)) {
            return conclusionCache.get(conclWrapper);
        } else {
            return null;
        }
    }

    @Override
    public void addToCache(Conclusion conclusion) {
        if (conclusionCache == null) {
            checkCache(conclusion);
        }

        if (conclusion.isIsClosed()) {
            conclusionCache.remove(new ComparableConclusionWrapper(conclusion));
            return;
        }

        logger.debug("Adding to cache:");
        logger.debug(conclusion.debugPrint());

        //conclusion = (Conclusion) conclusionDao.getEntityDao().refreshEntity(conclusion);
        conclusionCache.put(new ComparableConclusionWrapper(conclusion), conclusion);
        // go through the children and add any child conclusions to the cache as well
        for (ConclusionTrigger trigger: conclusion.getTriggers()) {
            for (ConclusionTriggerSource triggerSource: trigger.getSources()) {
                Conclusion childConclusion = triggerSource.getConclusion();
                if (childConclusion != null) {
                    addToCache(childConclusion);
                }
            }
        }
    }

    @Override
    public void removeFromCache(Conclusion conclusion) {
        if (conclusionCache == null) {
            checkCache(conclusion);
        }

        conclusionCache.remove(new ComparableConclusionWrapper(conclusion));
    }

    @Override
    public void clear() {
        conclusionCache = null;
    }


    public class ComparableConclusionWrapper {

        private Conclusion conclusion;

        private Long componentId;
        private int hash;

        public ComparableConclusionWrapper(Conclusion conclusion) {
            this.conclusion = conclusion;
            this.componentId = conclusion.getComponent().getId();

            ConclusionType type = conclusion.getType();

            hash = 7;
            hash = 19 * hash + (type != null ? type.getid().hashCode() : 0);
            hash = 19 * hash + (componentId != null ? componentId.hashCode() : 0);
        }

        @Override
        public int hashCode() {
            return hash;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj == null) {
                return false;
            }
            if (getClass() != obj.getClass()) {
                return false;
            }
            Conclusion otherConcl = ((ComparableConclusionWrapper) obj).getConclusion();
            if (!conclusion.getType().getid().equals(otherConcl.getType().getid())) {
                return false;
            }

            if (!componentId.equals(otherConcl.getComponent().getId())) {
                return false;
            }

//            if (!otherConcl.getTitle().equals(conclusion.getTitle())) {
//                return false;
//            }

//            if (!otherConcl.getDescription().equals(conclusion.getDescription())) {
//                return false;
//            }

            return true;
        }

        public Conclusion getConclusion() {
            return conclusion;
        }
    }

}
