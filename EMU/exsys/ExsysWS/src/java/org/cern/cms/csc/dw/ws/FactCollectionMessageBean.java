package org.cern.cms.csc.dw.ws;

import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.MessageDriven;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.fact.FactCollection;

@MessageDriven(mappedName="jms/factCollectionQueue", activationConfig =  {
    @ActivationConfigProperty(propertyName = "destinationType", propertyValue = "javax.jms.Queue"),
    @ActivationConfigProperty(propertyName = "acknowledgeMode", propertyValue = "Auto-acknowledge")
})
public class FactCollectionMessageBean implements MessageListener {

    private static Logger logger = SimpleLogger.getLogger(FactCollectionMessageBean.class);

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
        } catch (Exception ex) {
            logger.error("Exception in FactCollectionMessageBean", ex);
        }
    }
    
}
