package org.cern.cms.csc.dw.util;

import java.io.Serializable;
import java.util.Collection;
import java.util.Enumeration;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.QueueBrowser;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSender;
import javax.jms.QueueSession;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

public abstract class JmsWorker {

    private static final Logger logger = SimpleLogger.getLogger(JmsWorker.class);

    protected abstract Queue getQueue() throws Exception;
    protected abstract QueueConnectionFactory getQueueConnectionFactory() throws Exception;

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
        } catch (Exception ex) {
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
        } catch (Exception ex) {
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

    public Object flushObjectMessage() {
        QueueConnection connection = null;
        QueueSession session = null;
        try {
            connection = getQueueConnectionFactory().createQueueConnection();
            session = connection.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
            MessageConsumer consumer = session.createConsumer(getQueue());
            Message msg = consumer.receiveNoWait();
            if (msg != null && msg instanceof ObjectMessage) {
                return ((ObjectMessage) msg).getObject();
            }
        } catch (Exception ex) {
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

        return null;

    }

    public Integer getSize() {
        Integer num = 0;
        QueueConnection connection = null;
        QueueSession session = null;
        try {
            connection = getQueueConnectionFactory().createQueueConnection();
            session = connection.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
            QueueBrowser browser = session.createBrowser(getQueue());
            connection.start();
            Enumeration en = browser.getEnumeration();
            while (en.hasMoreElements()) {
                en.nextElement();
                num++;
            }
        } catch (Exception ex) {
            num = null;
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
        return num;
    }

    public String getName() {
        try {
            return getQueue().getQueueName();
        } catch (Exception ex) {
            logger.error(ex);
        }
        return null;
    }

}
