package org.cern.cms.csc.dw.ws;

import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;

@MessageDriven(mappedName="jms/factCollectionQueue", activationConfig =  {
    @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue"),
    @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge")
})
public class FactCollectionMessageBean implements MessageListener {

    private static Logger logger = ExsysLogger.getLogger(FactCollectionMessageBean.class);

    @EJB
    FactCollectionSaverLocal saver;

    public FactCollectionMessageBean() { }

    @Override
    public void onMessage(Message message) {
        try {
            ObjectMessage omessage = (ObjectMessage) message;
            FactCollection fc = (FactCollection) omessage.getObject();
            logger.debug("FC message bean: got a fact collection to message bean, sending to FactCollectionSaver");
            saver.saveFactCollection(fc);
        } catch (EmptyListReceivedException elrEx) {
            logger.warn("Empty fact collection received");
        } catch (Exception ex) {
            logger.error("Exception in FactCollectionMessageBean", ex);
        }
    }
    
}
