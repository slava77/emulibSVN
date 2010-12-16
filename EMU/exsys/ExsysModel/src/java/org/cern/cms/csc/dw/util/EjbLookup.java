package org.cern.cms.csc.dw.util;

import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class EjbLookup <T> {

    private static final String DEFAULT_APPLICATION = "Exsys";
    private static final String DEFAULT_MODULE = "ExsysModel";
    private static final String PATTERN = "java:global/%1$2s/%2$2s/%3$2s!%4$2s";

    private static final Logger logger = SimpleLogger.getLogger(EjbLookup.class);

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

    public T ejb() {
        if (this.ejb == null) {
            this.ejb = lookupEjb();
        }
        return this.ejb;
    }

    private T lookupEjb() {
        try {
            Context c = new InitialContext();
            return (T) c.lookup(String.format(PATTERN, applicationName, moduleName, ejbClass.getSimpleName(), ejbInterface.getName()));
        } catch (NamingException ne) {
            logger.error(ne);
            throw new RuntimeException(ne);
        }
    }


}
