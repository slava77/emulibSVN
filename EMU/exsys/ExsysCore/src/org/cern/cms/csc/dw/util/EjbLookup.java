package org.cern.cms.csc.dw.util;

import java.util.Properties;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;

/**
 * Lazy loading EJB wrapper, uses JNDI lookup.
 * @note For successful lookup using EJB interface only, all EJB interfaces should comply with this naming convention: <bean-name>Local/Remote
 * @author valdo
 */
public class EjbLookup <T> {

    public enum Module {

        DAO("ExsysDao"),
        RULE_ENGINE("ExsysRE"),
        WEB_SERVICE("ExsysWS"),
        IO("ExsysIO");

        private final String value;

        Module(String v) {
            value = v;
        }

        public String getValue() {
            return value;
        }
    }

    private static final Logger logger = ExsysLogger.getLogger(EjbLookup.class);
    private static final int DEFAULT_IIOP_PORT = 3700;
//    private static final String PATTERN = "java:global/%1$2s/%2$2s/%3$2s!%4$2s";
    private static final String PATTERN = "java:app/%s/%s!%s";

    private String remoteHostname = null;
    private int remoteIIOPPort = DEFAULT_IIOP_PORT;
    private Module ejbModule;
    private Class<T> ejbInterface;
    private boolean isRemote = false;

    private T ejb = null;

    /**
     * @param ejbInterface interface of the EJB that you want to lookup
     * @param ejbModule EJB module where the target EJB is residing
     */
    public EjbLookup(Module ejbModule, Class<T> ejbInterface) {
        this.ejbInterface = ejbInterface;
        this.ejbModule = ejbModule;
    }

    /**
     * Used for remote EJB lookup
     * @note remote lookup uses glassfish specific JNDI names, they're not portable
     * @param ejbInterface interface of the EJB that you want to lookup
     */
    public EjbLookup(Class<T> ejbInterface, String remoteHostname) {
        this(null, ejbInterface);
        this.remoteHostname = remoteHostname;
        this.isRemote = true;
    }

    /**
     * Used for remote EJB lookup
     * @note remote lookup uses glassfish specific JNDI names, they're not portable
     * @param ejbInterface interface of the EJB that you want to lookup
     */
    public EjbLookup(Class<T> ejbInterface, String remoteHostname, int remoteIIOPPort) {
        this(ejbInterface, remoteHostname);
        this.remoteIIOPPort = remoteIIOPPort;
        this.isRemote = true;
    }

    /**
     * Lazily looks up the EJB and returns it.
     * @return the EJB object.
     * @note this method is silent, meaning that if there's a NamingException, it will be wrapped into a RuntimeException,
     *         if you need to be able to handle the NamingException gracefully, use the ejbStrict() method
     */
    public T ejb() {
        try {
            return ejbStrict();
        } catch (NamingException ne) {
            logger.error(ne);
            throw new RuntimeException(ne);
        }
    }

    /**
     * Lazily looks up the EJB and returns it. This is called strict because it doesn't hide the NamingException and lets you handle it.
     * @return the EJB object.
     */
    public T ejbStrict() throws NamingException {
        if (this.ejb == null) {
            this.ejb = lookupEjb();
        }
        return this.ejb;
    }

    private T lookupEjb() throws NamingException {
        Context c;
        if (!isRemote) {
            c = new InitialContext();
            String beanName = ejbInterface.getSimpleName().replaceAll("Local|Remote", "");
            String jndiName = String.format(PATTERN, ejbModule.getValue(), beanName, ejbInterface.getName());
            return (T) c.lookup(jndiName);
        } else {
            Properties props = new Properties();
            props.setProperty("org.omg.CORBA.ORBInitialHost", remoteHostname);
            props.setProperty("org.omg.CORBA.ORBInitialPort", Integer.toString(remoteIIOPPort));
            c = new InitialContext(props);
            return (T) c.lookup(ejbInterface.getCanonicalName()); // glassfish specific
        }
    }


}