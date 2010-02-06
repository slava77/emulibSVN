package org.cern.cms.csc.dw.ws;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.ejb.EJB;
import org.cern.cms.csc.dw.model.fact.FactCollection;
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
import org.cern.cms.csc.dw.service.ServiceInstructions;

@WebService(serviceName = "cdw", name = "factcollection")
@Stateless()
public class FactCollectionInput {

    private static Logger logger = Logger.getLogger(FactCollectionInput.class.getName());
    @EJB
    FactCollectionSaverLocal saver;
    @Resource(mappedName = "jms/factCollestionQueue")
    private Queue queue;
    @Resource(mappedName = "jms/factCollestionQueueFactory")
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
                saver.saveFactCollection(factCollection);
            }
        } catch (Exception ex) {
            logger.log(Level.SEVERE, (String) null, ex);
            throw ex;
        }

        return factCollection.getFacts().size();

    }
}
