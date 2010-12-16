package org.cern.cms.csc.dw.util;

import java.beans.BeanInfo;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import javax.naming.Reference;
import java.lang.reflect.Method;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;
import javax.naming.Context;
import javax.naming.Name;
import javax.naming.NamingException;
import javax.naming.RefAddr;
import javax.naming.spi.ObjectFactory;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;

public class GenericImmutableSingletonFactory implements ObjectFactory {

    private static final Logger logger = SimpleLogger.getLogger(GenericImmutableSingletonFactory.class);
    private static Map<Name, Object> singletons = new HashMap<Name, Object>();

    public Object getObjectInstance(Object obj, Name name, Context nameCtx,
            Hashtable environment) throws NamingException {
        logger.debug("obj=" + obj);
        logger.debug("name=" + name);
        logger.debug("nameCtx=" + nameCtx);
        if (obj instanceof Reference) {
            Reference ref = (Reference) obj;
            String beanClassName = ref.getClassName();
            Class beanClass = null;
            ClassLoader tcl =
                    Thread.currentThread().getContextClassLoader();
            if (tcl != null) {
                try {
                    beanClass = tcl.loadClass(beanClassName);
                } catch (ClassNotFoundException e) {
                }
            } else {
                try {
                    beanClass = Class.forName(beanClassName);
                } catch (ClassNotFoundException e) {
                    e.printStackTrace();
                }
            }
            if (beanClass == null) {
                throw new NamingException("Class not found: " + beanClassName);
            }

            Object bean;
            if (singletons.containsKey(name)) {
                logger.debug("returning already existing singleton: " + name);
                bean = singletons.get(name);
                if (!bean.getClass().equals(beanClass)) {
                    // should probably recreate the singleton here
                    throw new NamingException("Singleton with name \"" + name +
                            "\" is already registered, but is of different class. " +
                            "Registered singleton class: " + bean.getClass().getCanonicalName() +
                            " Requested class: " + beanClass.getCanonicalName());
                }
            } else {
                bean = createSingleton(beanClass, ref);
                singletons.put(name, bean);
            }
            return bean;

        } else {
            return null;
        }

    }

    private Object createSingleton(Class beanClass, Reference ref) throws NamingException {
        logger.info("Creating new instance of singleton of class " + beanClass);
        try {
            BeanInfo bi = Introspector.getBeanInfo(beanClass);
            PropertyDescriptor[] pda = bi.getPropertyDescriptors();

            Object bean = beanClass.newInstance();

            Enumeration e = ref.getAll();
            while (e.hasMoreElements()) {

                RefAddr ra = (RefAddr) e.nextElement();
                String propName = ra.getType();

                String value = (String) ra.getContent();

                Object[] valueArray = new Object[1];

                int i = 0;
                for (i = 0; i < pda.length; i++) {

                    if (pda[i].getName().equals(propName)) {

                        Class propType = pda[i].getPropertyType();

                        if (propType.equals(String.class)) {
                            valueArray[0] = value;
                        } else if (propType.equals(Character.class) || propType.equals(char.class)) {
                            valueArray[0] = Character.valueOf(value.charAt(0));
                        } else if (propType.equals(Byte.class) || propType.equals(byte.class)) {
                            valueArray[0] = Byte.valueOf(value);
                        } else if (propType.equals(Short.class) || propType.equals(short.class)) {
                            valueArray[0] = Short.valueOf(value);
                        } else if (propType.equals(Integer.class) || propType.equals(int.class)) {
                            valueArray[0] = Integer.valueOf(value);
                        } else if (propType.equals(Long.class) || propType.equals(long.class)) {
                            valueArray[0] = Long.valueOf(value);
                        } else if (propType.equals(Float.class) || propType.equals(float.class)) {
                            valueArray[0] = Float.valueOf(value);
                        } else if (propType.equals(Double.class) || propType.equals(double.class)) {
                            valueArray[0] = Double.valueOf(value);
                        } else {
                            throw new NamingException("String conversion for property type '" + propType.getName() + "' not available");
                        }

                        Method setProp = pda[i].getWriteMethod();
                        if (setProp != null) {
                            setProp.invoke(bean, valueArray);
                        } else {
                            throw new NamingException("Write not allowed for property: " + propName);
                        }

                        break;

                    }

                }

                if (i == pda.length) {
                    throw new NamingException("No set method found for property: " + propName);
                }

            }

            Method[] methods = beanClass.getMethods();
            for (Method method : methods) {
                if (method.getName().equals("init")) {
                    logger.info("Init method found in class " + beanClass.getName() + " - calling");
                    method.invoke(bean);
                }
            }

            return bean;

        } catch (java.beans.IntrospectionException ie) {
            throw new NamingException(ie.getMessage());
        } catch (java.lang.IllegalAccessException iae) {
            throw new NamingException(iae.getMessage());
        } catch (java.lang.IllegalArgumentException iarge) {
            throw new NamingException(iarge.getMessage());
        } catch (java.lang.InstantiationException ie2) {
            throw new NamingException(ie2.getMessage());
        } catch (java.lang.reflect.InvocationTargetException ite) {
            throw new NamingException(ite.getMessage());
        }
    }
}
