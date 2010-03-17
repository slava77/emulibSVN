/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.io.Serializable;
import javax.persistence.EntityManager;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;

/**
 *
 * @author Evka
 */
public class EntityBase implements Serializable {

    public void onSave(EntityManager em) throws OnSaveProcessingException { }
    public void onReceive() throws OnReceiveProcessingException { }
}
