package org.cern.cms.csc.dw.util;

import java.util.Properties;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class EjbLookup <T> {

    private static final String DEFAULT_APPLICATION = "Exsys";
    private static final String DEFAULT_MODULE = "ExsysModel";
    private static final int DEFAULT_IIOP_PORT = 3700;
    private static final String PATTERN = "java:global/%1$2s/%2$2s/%3$2s!%4$2s";

    private static final Logger logger = SimpleLogger.getLogger(EjbLookup.class);

    private String remoteHostname = null;
    private int remoteIIOPPort = DEFAULT_IIOP_PORT;
    private String applicationName = DEFAULT_APPLICATION;
    private String moduleName = DEFAULT_MODULE;
    private Class<? extends T> ejbClass;
    private Class<T> ejbInterface;

    private T ejb = null;
    
    public EjbLookup(Class<T> ejbInterface, Class<? extends T> ejbClass) {
        this.ejbInterface = ejbInterface;
        this.ejbClass = ejbClass;
    }

    public EjbLookup(Class<T> ejbInterface, Class<? extends T> ejbClass, String moduleName) {
        this(ejbInterface, ejbClass);
        this.moduleName = moduleName;
    }

    public EjbLookup(Class<T> ejbInterface, Class<? extends T> ejbClass, String applicationName, String moduleName) {
        this(ejbInterface, ejbClass, moduleName);
        this.applicationName = applicationName;
    }

    public EjbLookup(Class<T> ejbInterface, Class<? extends T> ejbClass, String applicationName, String moduleName, String remoteHostname) {
        this(ejbInterface, ejbClass, applicationName, moduleName);
        this.remoteHostname = remoteHostname;
    }

    public EjbLookup(Class<T> ejbInterface, Class<? extends T> ejbClass, String applicationName, String moduleName, String remoteHostname, int remoteIIOPPort) {
        this(ejbInterface, ejbClass, applicationName, moduleName, remoteHostname);
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
            return (T) c.lookup(String.format(PATTERN, applicationName, moduleName, ejbClass.getSimpleName(), ejbInterface.getName()));
        } catch (NamingException ne) {
            logger.error(ne);
            throw new RuntimeException(ne);
        }
    }


}
