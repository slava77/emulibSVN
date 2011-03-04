/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.util;

import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheService;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheServiceLocal;

/**
 *
 * @author evka
 */
public class Exsys {

    private static Logger logger = SimpleLogger.getLogger(Exsys.class);

    private static EjbLookup<ConclusionCacheServiceLocal> conclCache = new EjbLookup(ConclusionCacheServiceLocal.class,
                                                                                    ConclusionCacheService.class,
                                                                                    "ExsysRE");

    public static boolean conclusionExists(String conclusionTypeName, Component component) {
        try {
            logger.info("Hey - got a conclusionExists() call with " + conclusionTypeName + " and " + component.getName());
            logger.info("returning " + (conclCache.ejb().checkCache(conclusionTypeName, component) != null));
            return (conclCache.ejb().checkCache(conclusionTypeName, component) != null);
        } catch (Throwable th) {
            logger.error(th);
            return false;
        }
    }

}
