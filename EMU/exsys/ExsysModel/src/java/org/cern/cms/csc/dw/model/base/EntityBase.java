/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.io.Serializable;
import jsf.bean.gui.EntityBeanBase;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;

/**
 * This is a base class of all entity beans used in this project.
 * @author Evka
 */
//@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class EntityBase extends EntityBeanBase implements Serializable {

    public void onSave(EntityDaoLocal eDao, boolean queued) throws OnSaveProcessingException { }
    public void onReceive(EntityDaoLocal eDao) throws OnReceiveProcessingException { }

}
