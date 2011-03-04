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
import org.cern.cms.csc.dw.model.ontology.Component;
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

    @Override
    public Conclusion checkCache(String conclusionTypeName, Component component) {
        checkIfCacheInitialized();
        return conclusionCache.get(new ComparableConclusionWrapper(conclusionTypeName, component));
    }

    /**
     * Checks if the given conclusion exists in the cache of open conclusions.
     * If it does, then returns the original conclusion that was found in the cache.
     * If it doesn't, returns null.
     */
    @Override
    public Conclusion checkCache(Conclusion conclusion) {
        checkIfCacheInitialized();
        return conclusionCache.get(new ComparableConclusionWrapper(conclusion));
    }

    private void checkIfCacheInitialized() {
        if (conclusionCache == null) { // initialize the cache
            conclusionCache = Collections.synchronizedMap(new HashMap<ComparableConclusionWrapper, Conclusion>());
            List<Conclusion> conclusions = reDao.getAllOpenConclusions();
            for (Conclusion concl: conclusions) {
                conclusionCache.put(new ComparableConclusionWrapper(concl), concl);
            }
        }
    }

    @Override
    public void addToCache(Conclusion conclusion) {
        checkIfCacheInitialized();

        if (conclusion.isClosed()) {
            conclusionCache.remove(new ComparableConclusionWrapper(conclusion));
            return;
        }

        logger.debug("Adding to cache:");
        logger.debug(conclusion.debugPrint());

        //conclusion = (Conclusion) conclusionDao.getEntityDao().refreshEntity(conclusion);
        conclusionCache.put(new ComparableConclusionWrapper(conclusion), conclusion);
        // go through the children and add any child conclusions to the cache as well
//        for (ConclusionTrigger trigger: conclusion.getTriggers()) {
//            for (ConclusionTriggerSource triggerSource: trigger.getSources()) {
//                Conclusion childConclusion = triggerSource.getConclusion();
//                if ((childConclusion != null) && !childConclusion.isClosed()) {
//                    addToCache(childConclusion);
//                }
//            }
//        }
    }

    @Override
    public void removeFromCache(Conclusion conclusion) {
        checkIfCacheInitialized();

        conclusionCache.remove(new ComparableConclusionWrapper(conclusion));
    }

    @Override
    public void clear() {
        conclusionCache = null;
    }


    public class ComparableConclusionWrapper {

        private String conclusionTypeName;

        private Long componentId;
        private int hash;

        public ComparableConclusionWrapper(Conclusion conclusion) {
            this(conclusion.getType().getName(), conclusion.getComponent());
        }

        public ComparableConclusionWrapper(String conclusionTypeName, Component component) {
            this.conclusionTypeName = conclusionTypeName;
            this.componentId = component.getId();

            hash = 7;
            hash = 19 * hash + (conclusionTypeName != null ? conclusionTypeName.hashCode() : 0);
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
            ComparableConclusionWrapper otherConclWrapper = (ComparableConclusionWrapper) obj;
            if (!conclusionTypeName.equals(otherConclWrapper.getConclusionTypeName())) {
                return false;
            }

            if (!componentId.equals(otherConclWrapper.getComponentId())) {
                return false;
            }

            return true;
        }

        public Long getComponentId() {
            return componentId;
        }

        public String getConclusionTypeName() {
            return conclusionTypeName;
        }

    }

}
