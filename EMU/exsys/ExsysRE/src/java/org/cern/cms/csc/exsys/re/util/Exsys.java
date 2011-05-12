/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.util;

import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheServiceLocal;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author evka
 */
public class Exsys {

    private static Logger logger = ExsysLogger.getLogger(Exsys.class);

    private static EjbLookup<ConclusionCacheServiceLocal> conclCache = new EjbLookup(EjbLookup.Module.RULE_ENGINE,
                                                                                   ConclusionCacheServiceLocal.class);

    public static boolean conclusionExists(String conclusionTypeName, Component component) {
        try {
            return (conclCache.ejb().checkCache(conclusionTypeName, component) != null);
        } catch (Throwable th) {
            logger.error(th);
            return false;
        }
    }

    public static Conclusion getConclusion(String conclusionTypeName, Component component) {
        try {
            return conclCache.ejb().checkCache(conclusionTypeName, component);
        } catch (Throwable th) {
            logger.error(th);
            return null;
        }
    }

}
