/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

import javax.jms.Queue;
import javax.jms.QueueConnectionFactory;
import javax.naming.NamingException;

/**
 *
 * @author valdo
 */
public class JmsWorkerJni extends JmsWorker {

    private final String name;

    public JmsWorkerJni(String name) throws NamingException {
        this.name = name;
        if (!isJmsService(name)) {
            throw new NamingException(name + " is not a JMS Queue and Factory name!");
        }
    }

    @Override
    protected Queue getQueue() throws Exception {
        return (Queue) getJmsService(name);
    }

    @Override
    protected QueueConnectionFactory getQueueConnectionFactory() throws Exception {
        return (QueueConnectionFactory) getJmsService(name + "Factory");
    }

    private static Object getJmsService(String serviceName) throws NamingException {
        return ServiceLocator.getInstance().getService("jms/" + serviceName);
    }

    public static boolean isJmsService(String serviceName) {
        try {
            return (getJmsService(serviceName) instanceof Queue &&
                    getJmsService(serviceName + "Factory") instanceof QueueConnectionFactory);
        } catch (NamingException ex) {
            return false;
        }
    }

}
