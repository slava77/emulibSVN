/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;
import org.cern.cms.csc.dw.model.fact.FactCollection;

/**
 *
 * @author valdo
 */
@Local
public interface FactDaoLocal {

    void saveFactCollection(final FactCollection factCollection) throws Exception;
    
}
