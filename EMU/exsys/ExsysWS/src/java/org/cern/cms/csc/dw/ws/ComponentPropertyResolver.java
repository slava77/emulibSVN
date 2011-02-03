package org.cern.cms.csc.dw.ws;

import java.beans.PropertyDescriptor;
import java.lang.reflect.Method;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import javax.ejb.EJB;
import javax.ejb.Singleton;
import org.apache.commons.beanutils.PropertyUtils;
import org.apache.commons.lang.NullArgumentException;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.exception.WrongComponentTypeException;
import org.cern.cms.csc.dw.model.annotation.ComponentId;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.ws.exception.ComponentNotProvidedException;

/**
 * Resovles object properties that have Component type.
 * Object must have the following properties"
 *
 *  @Transient
 *  @ComponentId(forProperty = "myComponent", limitComponents = { "Chamber" }, limitComponentsRecursive = false, required = true)
 *  public String getMyComponentId();
 *  public void setMyComponentId(String myComponentId);
 *
 *  public Component getMyComponent();
 *  public void setMyComponent(Component myComponent);
 *
 */
@Singleton
public class ComponentPropertyResolver implements ComponentPropertyResolverLocal {

    @EJB
    private OntologyDaoLocal ontologyDao;

    @EJB
    private GOntologyDaoLocal gOntologyDao;

    /**
     * Cached resolvers
     */
    private Map<Class, Set<ComponentPropertyResolverItem>> cache
            = Collections.synchronizedMap(new HashMap<Class, Set<ComponentPropertyResolverItem>>());

    /**
     * Solve component finding and assigning task for the provided object
     * @param bean Object to solve components for
     * @return object with solved components
     * @throws Exception
     */
    @Override
    public Object resolveComponentLinks(Object bean) throws Exception {

        Class clazz = bean.getClass();

        // If class data is not in the cache - put it.
        // One for each method + component
        if (!cache.containsKey(clazz)) {
            cache.put(clazz, new HashSet<ComponentPropertyResolverItem>());
            for (Method m: bean.getClass().getMethods()) {
                if (m.getReturnType().equals(String.class)) {
                    if (m.isAnnotationPresent(ComponentId.class)) {
                        ComponentId annotation = m.getAnnotation(ComponentId.class);
                        String componentProperty = annotation.forProperty();
                        if (PropertyUtils.getPropertyDescriptor(bean, componentProperty).getPropertyType().equals(Component.class)) {
                            cache.get(clazz).add(new ComponentPropertyResolverItem(clazz, m, componentProperty, annotation));
                        }
                    }
                }
            }
        }

        // Process components...
        if (!cache.get(clazz).isEmpty()) {
            for (ComponentPropertyResolverItem item: cache.get(clazz)) {
                bean = item.resolve(bean);
            }
        }

        return bean;
    }

    /**
     * Check if component type is valid for the parents provided
     * @param component Component to check
     * @param checkParents List of possible parent classes
     * @param checkRecursive If to be checked recursive?
     * @return true if provided component class is valid, false - otherwise
     * @throws ComponentNotFoundException
     */
    @Override
    public boolean checkComponentType(Component component, Set<ComponentClassType> checkParents, boolean checkRecursive)
            throws ComponentNotFoundException {
        return checkComponentType(gOntologyDao.getGComponent(component.getName()), checkParents, checkRecursive);
    }

    /**
     * Check if gcomponent type is valid for the parents provided
     * @param gcomponent GComponent to check
     * @param checkParents List of possible parent classes
     * @param checkRecursive If to be checked recursive?
     * @return true if provided component class is valid, false - otherwise
     */
    @Override
    public boolean checkComponentType(GComponent gcomponent, Set<ComponentClassType> checkParents, boolean checkRecursive) {
        return gOntologyDao.isGComponentClassParent(
            checkParents,
            gcomponent.getType(),
            checkRecursive);
    }

    /**
     * A single class property resolver
     */
    private class ComponentPropertyResolverItem {

        /**
         * Object class
         */
        private final Class clazz;

        /**
         * Method that holds component id property
         */
        private final Method componentIdMethod;

        /**
         * Component id property name
         */
        private String componentIdProperty = null;

        /**
         * Component property name
         */
        private final String componentProperty;

        /**
         * If component is required?
         */
        private final boolean required;

        /**
         * If component class limit check should be recursive?
         */
        private final boolean limitComponentsRecursive;

        /**
         * Component classes that this component must match to
         */
        private final Set<ComponentClassType> limitComponents;

        /**
         * Constructor
         * @param clazz Object class
         * @param componentIdMethod Method that holds component id
         * @param componentProperty Component property name
         * @param componentId Annotation
         */
        public ComponentPropertyResolverItem(Class clazz, Method componentIdMethod, String componentProperty, ComponentId componentId) {
            this.clazz = clazz;
            this.componentIdMethod = componentIdMethod;
            for (PropertyDescriptor pd: PropertyUtils.getPropertyDescriptors(clazz)) {
                if (pd.getReadMethod().equals(this.componentIdMethod)) {
                    this.componentIdProperty = pd.getName();
                    break;
                }
            }
            this.componentProperty = componentProperty;
            this.required = componentId.required();
            this.limitComponents = new HashSet<ComponentClassType>();
            for (String componentClassTypeName: componentId.limitComponents()) {
                limitComponents.add(ComponentClassType.fromValue(componentClassTypeName));
            }
            this.limitComponentsRecursive = componentId.limitComponentsRecursive();
        }

        /**
         * Resolve a single objects component property
         * @param bean Object to solve property for
         * @return Object with solved component property
         * @throws Exception
         */
        public Object resolve(Object bean) throws Exception {

            GComponent gcomponent = null;
            Component component = (Component) PropertyUtils.getSimpleProperty(bean, componentProperty);
            if (component != null) {
                gcomponent = gOntologyDao.getGComponent(component.getName());
            } else {
                String componentName = (String) componentIdMethod.invoke(bean);
                if (componentName != null) {
                    gcomponent = gOntologyDao.getGComponent(componentName);
                }
            }

            if (gcomponent != null) {
                if (!limitComponents.isEmpty()) {
                    if (!checkComponentType(gcomponent, limitComponents, limitComponentsRecursive)) {
                        throw new WrongComponentTypeException(gcomponent, clazz, componentProperty);
                    }
                }
                PropertyUtils.setSimpleProperty(bean, componentProperty, ontologyDao.getComponent(gcomponent));
                if (componentIdProperty != null) {
                    PropertyUtils.setSimpleProperty(bean, componentIdProperty, gcomponent.getName());
                }

            } else {
                if (required) {
                    throw new ComponentNotProvidedException(clazz, componentProperty);
                }
            }

            return bean;
        }

    }

}
