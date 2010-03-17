
package org.cern.cms.csc.dw.ws;

import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.Session;
import javax.xml.bind.JAXBElement;
import org.cern.cms.csc.dw.exception.ComponentTypeNotAllowedInFactException;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.service.ServiceInstructions;

@Stateless
public class FactCollectionSaverBean implements FactCollectionSaverLocal {
    @Resource(name = "jms/entitySaverQueue")
    private Queue entitySaverQueue;
    @Resource(name = "jms/entitySaverQueueFactory")
    private ConnectionFactory entitySaverQueueFactory;

    private static Logger logger = Logger.getLogger(FactCollectionSaverBean.class.getName());

    @EJB
    private OntologyDaoLocal ontologyDao;

    public void saveFactCollection(FactCollection factCollection) throws Exception {

        Set<JAXBElement<? extends Fact>> toRemove = new HashSet<JAXBElement<? extends Fact>>();
        ServiceInstructions instructions = factCollection.getServiceInstructions();

        // Clean factCollectionFactsItems
        factCollection.getFactsItems().clear();

        // Loop over collection facts
        for (JAXBElement<? extends Fact> fi: factCollection.getFacts()) {

            // Get a fact
            Fact fact = fi.getValue();

            // Get ontology component object from fact component id
            try {

                Component component = ontologyDao.getComponentById(fact.getComponentId());
                if (!fact.checkComponentClassType(component.getComponentClass().getId())) {
                    throw new ComponentTypeNotAllowedInFactException(component.getComponentClass().getId(), fact.getClass());
                }
                fact.setComponent(component);
                fact.setComponentId(component.getId());

                fact.onReceive();

                // This fact is OK so we add it to factItems
                FactCollectionFactsItem fcfi = new FactCollectionFactsItem();
                fcfi.setItemValue(fact);
                factCollection.getFactsItems().add(fcfi);

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
        for (JAXBElement<? extends Fact> fi: toRemove) {
            factCollection.getFacts().remove(fi);
        }

        if (!factCollection.isSetFacts()) {
            throw new EmptyListReceivedException("factCollection", "Fact");
        }

        // Persist collection
        if (instructions.isPersist()) {
            sendJMSMessageToEntitySaverQueue(factCollection);
        }

    }

    private Message createJMSMessageForjmsEntitySaverQueue(Session session, EntityBase entity) throws JMSException {
        ObjectMessage om = session.createObjectMessage();
        om.setObject(entity);
        return om;
    }

    private void sendJMSMessageToEntitySaverQueue(EntityBase entity) throws JMSException {
        Connection connection = null;
        Session session = null;
        try {
            connection = entitySaverQueueFactory.createConnection();
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            MessageProducer messageProducer = session.createProducer(entitySaverQueue);
            messageProducer.send(createJMSMessageForjmsEntitySaverQueue(session, entity));
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
