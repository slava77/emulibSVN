package org.cern.cms.csc.dw.model.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface FactAnn {

    /**
     * Fact title. Used in OLAP Cube name and elsewhere
     * @return
     */
    String title();

    /**
     * Types of component classes this fact can have. Empty for any.
     * List of strings should refer to org.cern.cms.csc.dw.model.ontology.ComponentClassType!!!
     * Array of enums is not supported by JAXB?
     */
    String [] limitComponents() default {};

    /**
     * Should limitComponents be looked at recursively?
     */
    boolean limitComponentsRecursive() default false;

    /**
     * Number of days to store in DB. 0 = unilimited.
     */
    int daysToStore() default 0;

}
