/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import javax.ejb.Local;
import org.cern.cms.csc.dw.model.fact.FactCollection;

/**
 *
 * @author valdo
 */
@Local
public interface FactCollectionSaverLocal {

    public void saveFactCollection(FactCollection factCollection) throws java.lang.Exception;
    
}
