package org.cern.cms.csc.dw.util;

import java.io.Serializable;
import java.util.Collection;
import javax.jms.JMSException;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSender;
import javax.jms.QueueSession;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

public abstract class MessageSender {

    private static final Logger logger = SimpleLogger.getLogger(MessageSender.class);

    protected abstract Queue getQueue();
    protected abstract QueueConnectionFactory getQueueConnectionFactory();

    public void sendMessages(Collection<Serializable> objCol) {
        QueueConnection connection = null;
        QueueSession session = null;
        try {
            connection = getQueueConnectionFactory().createQueueConnection();
            session = connection.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
            QueueSender sender = session.createSender(getQueue());
            for (Serializable obj: objCol) {
                ObjectMessage m = session.createObjectMessage();
                m.setObject(obj);
                sender.send(m);
            }
        } catch (JMSException ex) {
            logger.error(ex);
        } finally {
            if (session != null) {
                try {
                    session.close();
                } catch (JMSException ex) {
                    logger.warn(ex);
                }
            }
            if (session != null) {
                try {
                    connection.close();
                } catch (JMSException ex) {
                    logger.warn(ex);
                }
            }
        }
    }

    public void sendMessage(Serializable obj) {
        QueueConnection connection = null;
        QueueSession session = null;
        try {
            connection = getQueueConnectionFactory().createQueueConnection();
            session = connection.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
            QueueSender sender = session.createSender(getQueue());
            ObjectMessage m = session.createObjectMessage();
            m.setObject(obj);
            sender.send(m);
        } catch (JMSException ex) {
            logger.error(ex);
        } finally {
            if (session != null) {
                try {
                    session.close();
                } catch (JMSException ex) {
                    logger.warn(ex);
                }
            }
            if (session != null) {
                try {
                    connection.close();
                } catch (JMSException ex) {
                    logger.warn(ex);
                }
            }
        }
    }

}
