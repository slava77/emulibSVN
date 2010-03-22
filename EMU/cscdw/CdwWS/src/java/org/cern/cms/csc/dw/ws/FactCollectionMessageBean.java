package org.cern.cms.csc.dw.ws;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import org.cern.cms.csc.dw.model.fact.FactCollection;

@MessageDriven(mappedName = "jms/factCollectionQueue", activationConfig =  {
        @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge"),
        @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue")
    })
public class FactCollectionMessageBean implements MessageListener {

    private static Logger logger = Logger.getLogger(FactCollectionMessageBean.class.getName());

    @EJB
    FactCollectionSaverLocal saver;

    public FactCollectionMessageBean() { }

    public void onMessage(Message message) {
        try {
            ObjectMessage omessage = (ObjectMessage) message;
            FactCollection fc = (FactCollection) omessage.getObject();
            logger.finest("FC message bean: got a fact collection to message bean, sending to FactCollectionSaver");
            saver.saveFactCollection(fc);
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception in FactCollectionMessageBean", ex);
        }
    }
    
}
