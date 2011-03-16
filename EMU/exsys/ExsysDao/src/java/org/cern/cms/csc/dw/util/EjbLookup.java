package org.cern.cms.csc.dw.util;

import java.util.Properties;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 * Lazy loading EJB wrapper, uses JNDI lookup.
 * @note Used JNDI names are glassfish specific and not portable
 * (but enables to lookup EJBs only by the interface when the implementation class is unknown or inaccessible)
 * @author valdo
 */
public class EjbLookup <T> {

    private static final int DEFAULT_IIOP_PORT = 3700;

    private static final Logger logger = SimpleLogger.getLogger(EjbLookup.class);

    private String remoteHostname = null;
    private int remoteIIOPPort = DEFAULT_IIOP_PORT;
    private Class<T> ejbInterface;

    private T ejb = null;

    /**
     * @param ejbInterface interface of the EJB that you want to lookup
     */
    public EjbLookup(Class<T> ejbInterface) {
        this.ejbInterface = ejbInterface;
    }

    /**
     * Used for remote EJB lookup
     * @param ejbInterface interface of the EJB that you want to lookup
     */
    public EjbLookup(Class<T> ejbInterface, String remoteHostname) {
        this(ejbInterface);
        this.remoteHostname = remoteHostname;
    }

    /**
     * Used for remote EJB lookup
     * @param ejbInterface interface of the EJB that you want to lookup
     */
    public EjbLookup(Class<T> ejbInterface, String remoteHostname, int remoteIIOPPort) {
        this(ejbInterface, remoteHostname);
        this.remoteIIOPPort = remoteIIOPPort;
    }

    public T ejb() {
        if (this.ejb == null) {
            this.ejb = lookupEjb();
        }
        return this.ejb;
    }

    private T lookupEjb() {
        try {
            Context c;
            if (remoteHostname == null) {
                c = new InitialContext();
            } else {
                Properties props = new Properties();
                props.setProperty("org.omg.CORBA.ORBInitialHost", remoteHostname);
                props.setProperty("org.omg.CORBA.ORBInitialPort", Integer.toString(remoteIIOPPort));
                c = new InitialContext(props);
            }
            return (T) c.lookup(ejbInterface.getCanonicalName());
        } catch (NamingException ne) {
            logger.error(ne);
            throw new RuntimeException(ne);
        }
    }


}