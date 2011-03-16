package org.cern.cms.csc.dw.model.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface ComponentId {

    /**
     * Component property name in the same class
     * @return
     */
    String forProperty();

    /**
     * Is the component required? If yes, the Resolver will fire exception
     * if not found either component, either its' id. Otherwise it will quit
     * silently leaving component = null
     * @return
     */
    boolean required() default true;

    /**
     * Types of component classes this component can have. Empty for any.
     * List of strings should refer to org.cern.cms.csc.dw.model.ontology.ComponentClassType!!!
     * Array of enums is not supported by JAXB?
     */
    String [] limitComponents() default {};

    /**
     * Should limitComponents be looked at recursively?
     */
    boolean limitComponentsRecursive() default false;

}
