/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.io.Serializable;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;

/**
 *
 * @author Evka
 */
public class EntityBase implements Serializable {

    public void onSave(EntityDaoLocal eDao, boolean queued) throws OnSaveProcessingException { }

    public void onReceive(EntityDaoLocal eDao) throws OnReceiveProcessingException { }
    
}
