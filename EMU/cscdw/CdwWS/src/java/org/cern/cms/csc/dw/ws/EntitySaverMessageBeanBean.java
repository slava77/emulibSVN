/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.exception.NotImplementedException;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.base.EntityBase;

/**
 *
 * @author Evka
 */
@MessageDriven(mappedName = "jms/entitySaverQueue", activationConfig =  {
        @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge"),
        @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue")
    })
public class EntitySaverMessageBeanBean implements MessageListener {
    
    private static Logger logger = Logger.getLogger(EntitySaverMessageBeanBean.class.getName());

    @EJB
    private PersistDaoLocal persistDao;

    public EntitySaverMessageBeanBean() {
    }

    public void onMessage(Message message) {
        try {
            ObjectMessage objMsg = (ObjectMessage) message;
            Object entity = objMsg.getObject();
            if (entity instanceof EntityBase) {
                persistDao.persist((EntityBase) entity);
            } else {
                String className = "null";
                if (entity != null) {
                    className = objMsg.getObject().getClass().getCanonicalName();
                }
                logger.log(Level.SEVERE, className + " does not extend EntityBase class - cannot persist.");
            }
        } catch (PersistException pex) {
            logger.log(Level.WARNING, "Cannot persist entity, stopping persistance queue. ", pex);
            logger.log(Level.WARNING, "Queue stopping is not yet implemented. ", pex);
            //throw new NotImplementedException("Queue stopping is not yet implemented!");
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception while persisting an entity: ", ex);
        }
        //persistDao
    }
    
}
