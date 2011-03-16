package org.cern.cms.csc.dw.util;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import javax.naming.Binding;
import javax.naming.InitialContext;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;

public class ServiceLocator {

    /**
     * Singleton Instance of this class
     */
    private static ServiceLocator serviceLocator = null;
    
    /**
     * InitialContext object
     */
    InitialContext context = null;
    /**
     * Cache where the objects can be stored for later
     * retrieval.
     * This enhances the performance.
     */
    HashMap serviceCache = null;

    /**
     * Constructor to initialize the class
     *
     * @exception NamingException In case an exception is
     * generated
     */
    public ServiceLocator() throws NamingException {
        // Start the initial context
        context = new InitialContext();
        // Initialize the HashMap to store 5 objects
        serviceCache = new HashMap(5);
    }

    /**
     * Returns the singleton instance
     *
     * @exception NamingException In case an exception is
     * generated
     * @return Singleton Instance
     */
    public synchronized static ServiceLocator getInstance() throws NamingException {
        if (serviceLocator == null) {
            // If the object is not created, then create it
            serviceLocator = new ServiceLocator();
        }

        // Return the singleton instance
        return serviceLocator;
    }

    /**
     * This is the method that returns the service
     *
     * @param jndiName JNDI Lookup needed for invoking the
     * service
     * @exception NamingException In case an exception is
     * generated
     * @return Service Object
     */
    @SuppressWarnings("unchecked")
    public Object getService(String jndiName) throws NamingException {
        if (!serviceCache.containsKey(jndiName)) {
            // If the object is not saved in the cache, then do a
            //lookup and save it
            serviceCache.put(jndiName, context.lookup(jndiName));
        }
        // Return the required object
        return serviceCache.get(jndiName);
    }

    /**
     * Get EJB from same module or app. This is GF2.x way of getting EJB.
     * You need to hook EJB name in your consumer object by adding anotation (name injection)
     * i.e. @EJB(name="MyBeanRef", beanInterface=MyBeanLocal.class)
     * Then call
     * MyBeanLocal myBean = (MyBeanLocal) getEjb("MyBeanRef");
     * @param name Ejb name as defined in name injection
     * @return object found (please cast)
     * @throws javax.naming.NamingException (not found?)
     */
    public Object getEnvService(String name) throws NamingException {
        return getService("java:comp/env/" + name);
    }

    public Set<Binding> getJniBindings() throws NamingException {
        return getJniBindings("");
    }

    public Set<Binding> getJniBindings(String root) throws NamingException {
        Set<Binding> bindings = new HashSet<Binding>();
        NamingEnumeration<Binding> listBindings = context.listBindings(root);
        while (listBindings.hasMore()) {
            bindings.add(listBindings.next());
        }
        return bindings;
    }


}
