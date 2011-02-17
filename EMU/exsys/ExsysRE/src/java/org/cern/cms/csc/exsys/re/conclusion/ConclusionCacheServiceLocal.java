/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion;

import javax.ejb.Local;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author evka
 */
@Local
public interface ConclusionCacheServiceLocal {

    void addToCache(Conclusion conclusion);

    /**
     * Checks if the given conclusion exists in the cache of open conclusions.
     * If it does, then returns the original conclusion that was found in the cache.
     * If it doesn't, returns null.
     */
    Conclusion checkCache(Conclusion conclusion);

    void clear();

    void removeFromCache(Conclusion conclusion);

}
