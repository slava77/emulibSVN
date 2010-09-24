
package org.cern.cms.csc.dw.ws;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageProducer;
import javax.jms.Queue;
import javax.jms.Session;
import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.jms.ObjectMessage;
import javax.xml.bind.JAXBElement;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.exception.WrongComponentTypeException;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.service.ServiceInstructions;

@Stateless
public class FactCollectionSaverBean implements FactCollectionSaverLocal {

    private static Logger logger = Logger.getLogger(FactCollectionSaverBean.class.getName());

    @EJB
    private PersistDaoLocal persistDao;

    @EJB
    private ComponentPropertyResolverLocal componentResolver;

    @EJB
    private EntityDaoLocal entityDao;

    // rule engine queue stuff
    @Resource(name = "jms/ruleEngineInputQueue")
    private Queue ruleEngineInputQueue;
    @Resource(name = "jms/ruleEngineInputQueueFactory")
    private ConnectionFactory ruleEngineInputQueueFactory;

    public void saveFactCollection(FactCollection factCollection) throws Exception {
        logger.finest("FC Saver bean: at start, number of facts: " + factCollection.getFacts().size() + ", number of fis: " + factCollection.getFactsItems().size());

        Set<JAXBElement<? extends Fact>> toRemove = new HashSet<JAXBElement<? extends Fact>>();
        ServiceInstructions instructions = factCollection.getServiceInstructions();

        logger.info("FC Saver bean: received fact collection with " + factCollection.getFacts().size() + " facts");

        // Performing on receive operations
        factCollection.onReceive(entityDao);

        // Retrieving source gcomponent, checking its parents
        factCollection = (FactCollection) componentResolver.resolveComponentLinks(factCollection);

        // Loop over collection facts
        for (JAXBElement<? extends Fact> fi: factCollection.getFacts()) {

            // Get a fact
            Fact fact = fi.getValue();
            
            logger.fine("FC Saver bean: Processing fact: " + fact.toString());

            // Get ontology component object from fact component id or component.getId
            try {

                fact = (Fact) componentResolver.resolveComponentLinks(fact);

                if (!componentResolver.checkComponentType(
                        fact.getComponent(),
                        fact.getMetadata().getLimitComponents(),
                        fact.getMetadata().isLimitComponentsRecursive())) {
                    throw new WrongComponentTypeException(fact.getComponent(), fact.getClass(), "component");
                }

                fact.onReceive(entityDao);

                // This fact is OK so we add it to factItems
                //FactCollectionFactsItem fcfi = new FactCollectionFactsItem();
                //fcfi.setItemValue(fact);
                //factCollection.getFactsItems().add(fcfi);

            } catch (Exception ex) {
                if (instructions.isStrict()) {
                    throw ex;
                } else {
                    logger.log(Level.SEVERE, "Exception in FactCollectionSaverBean", ex);
                    toRemove.add(fi);
                }
            }

        }

        // Remove facts what need to be removed
        logger.finest("FC Saver bean: number of facts after processing but before removing: " + factCollection.getFacts().size());
        for (JAXBElement<? extends Fact> fi: toRemove) {
            factCollection.getFacts().remove(fi);
        }
        logger.finest("FC Saver bean: number of facts after processing and after removing: " + factCollection.getFacts().size());

        if (!factCollection.isSetFacts()) {
            throw new EmptyListReceivedException("factCollection", "Fact");
        }

        // Persist collection
        if (instructions.isPersist()) {
            logger.finest("FC Saver bean: serviceInstructions.isPersist() = true, so sending this fact collection to entity saver");
            persistDao.persist(factCollection);
        }

        try {
            Collection<Fact> factsToSendToRE = new ArrayList<Fact>();
            for (JAXBElement<? extends Fact> factEl: factCollection.getFacts()) {
                factsToSendToRE.add(factEl.getValue());
            }
            sendFactsToRuleEngineInputQueue(factsToSendToRE);
        } catch (JMSException jmsEx) {
            logger.log(Level.SEVERE, "Exception while sending fact to rule engine input queue", jmsEx);
        }
    }

    private Message createJMSMessageForjmsRuleEngineInputQueue(Session session, Serializable messageData) throws JMSException {
        // TODO create and populate message to send
        ObjectMessage om = session.createObjectMessage();
        om.setObject(messageData);
        return om;
    }

    private void sendFactsToRuleEngineInputQueue(Collection<Fact> facts) throws JMSException {
        Connection connection = null;
        Session session = null;
        try {
            connection = ruleEngineInputQueueFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            MessageProducer messageProducer = session.createProducer(ruleEngineInputQueue);
            for (Serializable fact: facts) {
                messageProducer.send(createJMSMessageForjmsRuleEngineInputQueue(session, fact));
            }
        } finally {
            if (session != null) {
                try {
                    session.close();
                } catch (JMSException e) {
                    Logger.getLogger(this.getClass().getName()).log(Level.WARNING, "Cannot close session", e);
                }
            }
            if (connection != null) {
                connection.close();
            }
        }
    }

}