package org.cern.cms.csc.dw.ws;

import org.cern.cms.csc.dw.ws.exception.NullArgumentReceivedException;
import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;
import javax.annotation.Resource;
import javax.ejb.EJB;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.fact.Fact;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import javax.jms.Queue;
import javax.jms.QueueConnectionFactory;
import javax.xml.bind.JAXBElement;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.monitor.MonitorFactCollectionLog;
import org.cern.cms.csc.dw.monitor.MonitorLogger;
import org.cern.cms.csc.dw.service.ServiceInstructions;
import org.cern.cms.csc.dw.util.JmsWorker;
import org.cern.cms.csc.dw.ws.exception.ComponentNotProvidedException;

@WebService(serviceName = "cdw", name = "factcollection", targetNamespace="http://www.cern.ch/cms/csc/dw/ws/factCollectionInput")
@Stateless
public class FactCollectionInput implements FactCollectionInputLocal {

    private static Logger logger = ExsysLogger.getLogger(FactCollectionInput.class);
    private static Logger monitor = MonitorLogger.getLogger(FactCollectionInput.class);

    @EJB
    FactCollectionSaverLocal saver;

    @Resource(mappedName = "jms/factCollectionQueue")
    private Queue queue;

    @Resource(mappedName = "jms/factCollectionQueueFactory")
    private QueueConnectionFactory queueConnectionFactory;

    private JmsWorker jmsWorker;

    public FactCollectionInput() {

        this.jmsWorker = new JmsWorker() {

            @Override
            protected Queue getQueue() {
                return queue;
            }

            @Override
            protected QueueConnectionFactory getQueueConnectionFactory() {
                return queueConnectionFactory;
            }
        };

    }

    /**
     * Receive and save fact collection
     * @param factCollection
     * @return number of facts saved
     * @throws java.lang.IllegalArgumentException
     */
    @WebMethod(operationName = "input")
    @Override
    public Integer getFactCollection(@WebParam(name = "factCollection") FactCollection factCollection) throws Exception {
        try {

            if (factCollection == null) {
                throw new NullArgumentReceivedException("factCollection");
            }

            // some debug information
            if (logger.isDebugEnabled()) {

                logger.debug("FCinput WS: fc with " + factCollection.getFacts().size() + " facts " +
                            " and " + factCollection.getFactsItems().size() + " fact collection items");
                logger.debug("FCinput WS: fc.toString(): " + factCollection.toString());
                for (JAXBElement<? extends Fact> fi : factCollection.getFacts()) {
                    logger.debug("FCinput WS: fact: " + fi.getValue());
                }
                
            }
            // -------------

            if (factCollection.getFactsItems().isEmpty()) {
                throw new EmptyListReceivedException("factCollection", "Fact");
            }

            if (factCollection.getSource() == null) {
                throw new ComponentNotProvidedException(FactCollection.class, "source");
            }

            monitor.info(new MonitorFactCollectionLog(factCollection));

            if (!factCollection.isSetServiceInstructions()) {
                factCollection.setServiceInstructions(new ServiceInstructions());
            }

            ServiceInstructions instructions = factCollection.getServiceInstructions();

            if (instructions.isAsync()) {
                jmsWorker.sendMessage(factCollection);
            } else {
                logger.debug("FCinput WS: serviceInstructions.isAsync() = false, sending this fact collection directly to FactCollectionSaverBean");
                saver.saveFactCollection(factCollection);
            }

        } catch (Exception ex) {
            logger.error("Error while saving Fact Collection", ex);
            throw ex;
        }

        return factCollection.getFacts().size();

    }

}
