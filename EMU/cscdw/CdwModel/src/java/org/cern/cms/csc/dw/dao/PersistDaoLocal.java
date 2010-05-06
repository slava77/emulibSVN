/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@Local
public interface PersistDaoLocal {

    public void persist(EntityBase cdwEntityObject) throws PersistException, Exception;
    public void persist(EntityBase cdwEntityObject, boolean queued) throws PersistException, java.lang.Exception;
   
}
