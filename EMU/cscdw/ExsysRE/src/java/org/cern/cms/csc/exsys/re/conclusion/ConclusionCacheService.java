/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionTrigger;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 *
 * @author evka
 */
public class ConclusionCacheService {

    private static final Logger logger = Logger.getLogger(ConclusionCacheService.class);

    private ConclusionDaoLocal conclusionDao;
    private Map<ComparableConclusionWrapper, Conclusion> conclusionCache;

    public ConclusionCacheService() {
    }

    public ConclusionCacheService(ConclusionDaoLocal conclusionDao) {
        this.conclusionDao = conclusionDao;
    }

    /**
     * Checks if the given conclusion exists in the cache of open conclusions.
     * If it does, then returns the original conclusion that was found in the cache.
     * If it doesn't, returns null.
     */
    public Conclusion checkCache(Conclusion conclusion) {
        if (conclusionCache == null) { // initialize the cache
            conclusionCache = new HashMap<ComparableConclusionWrapper, Conclusion>();
            List<Conclusion> conclusions = getConclusionDao().getAllOpenConclusions();
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

    public void addToCache(Conclusion conclusion) {
        if (conclusionCache == null) {
            checkCache(conclusion);
        }

        if (conclusion.isIsClosed()) {
            conclusionCache.remove(new ComparableConclusionWrapper(conclusion));
            return;
        }

        logger.info("Adding to cache:");
        logger.info(conclusion.debugPrint());

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

    public void removeFromCache(Conclusion conclusion) {
        if (conclusionCache == null) {
            checkCache(conclusion);
        }

        conclusionCache.remove(new ComparableConclusionWrapper(conclusion));
    }

    public void clear() {
        conclusionCache = null;
    }

    private ConclusionDaoLocal getConclusionDao() {
        if (conclusionDao == null) {
            Context jniContext;
            try {
                jniContext = new InitialContext();
                conclusionDao = (ConclusionDaoLocal) jniContext.lookup("java:comp/env/ejb/ConclusionDao");
            } catch (NamingException ex) {
                throw new RuntimeException("Naming exception while looking up conclusionDao ConclusionCacheService.getConclusionDao()", ex);
            }
        }
        return conclusionDao;
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
