package org.cern.cms.csc.dw.ws;

import org.cern.cms.csc.dw.ws.exception.NullArgumentReceivedException;
import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.ejb.EJB;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.fact.Fact;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSender;
import javax.jms.QueueSession;
import javax.xml.bind.JAXBElement;
import org.cern.cms.csc.dw.service.ServiceInstructions;

@WebService(serviceName = "cdw", name = "factcollection", targetNamespace="http://www.cern.ch/cms/csc/dw/ws/factCollectionInput")
@Stateless()
public class FactCollectionInput implements FactCollectionInputLocal {

    private static Logger logger = Logger.getLogger(FactCollectionInput.class.getName());
    @EJB
    FactCollectionSaverLocal saver;

    @Resource(mappedName = "jms/factCollectionQueue")
    private Queue queue;

    @Resource(mappedName = "jms/factCollectionQueueFactory")
    private QueueConnectionFactory queueConnectionFactory;

    /**
     * Receive and save fact collection
     * @param factCollection
     * @return number of facts saved
     * @throws java.lang.IllegalArgumentException
     */
    @WebMethod(operationName = "input")
    public Integer getFactCollection(@WebParam(name = "factCollection") FactCollection factCollection) throws Exception {
        try {

            if (factCollection == null) {
                throw new NullArgumentReceivedException("factCollection");
            }

            // some debug information
            logger.fine("FCinput WS: fc with " + factCollection.getFacts().size() + " facts " +
                        " and " + factCollection.getFactsItems().size() + " fact collection items");
            logger.finest("FCinput WS: fc.toString(): " + factCollection.toString());
            for (JAXBElement<? extends Fact> fi : factCollection.getFacts()) {
                logger.finest("FCinput WS: fact: " + fi.getValue());
            }
            // -------------

            if (factCollection.getFactsItems().isEmpty()) {
                throw new EmptyListReceivedException("factCollection", "Fact");
            }

            if (!factCollection.isSetServiceInstructions()) {
                factCollection.setServiceInstructions(new ServiceInstructions());
            }

            ServiceInstructions instructions = factCollection.getServiceInstructions();

            if (instructions.isAsync()) {
                final QueueConnection connection = queueConnectionFactory.createQueueConnection();
                final QueueSession session = connection.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
                ObjectMessage m = session.createObjectMessage();
                m.setObject(factCollection);
                final QueueSender sender = session.createSender(queue);
                sender.send(m);
                session.close();
                connection.close();
            } else {
                logger.info("FCinput WS: serviceInstructions.isAsync() = false, sending this fact collection directly to FactCollectionSaverBean");
                saver.saveFactCollection(factCollection);
            }

        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Error while saving Fact Collection", ex);
            throw ex;
        }

        return factCollection.getFacts().size();

    }
}
