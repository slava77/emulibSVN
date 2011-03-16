/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion;

import javax.ejb.Local;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author evka
 */
@Local
public interface ConclusionCacheServiceLocal {

    /**
     * Adds the given conclusion to the cache.
     */
    void addToCache(Conclusion conclusion);

    /**
     * Checks if the given conclusion exists in the cache of open conclusions.
     * If it does, then returns the original conclusion that was found in the cache.
     * If it doesn't, returns null.
     */
    Conclusion checkCache(Conclusion conclusion);

    /**
     * Checks if a conclusion with the given type name and component exists in the cache of open conclusions.
     * If it does, then returns the conclusion that was found in the cache.
     * If it doesn't, returns null.
     */
    Conclusion checkCache(String conclusionTypeName, Component component);

    /**
     * Clears the cache.
     */
    void clear();

    /**
     * Removes the given conclusion from the cache if it exists.
     */
    void removeFromCache(Conclusion conclusion);

}
