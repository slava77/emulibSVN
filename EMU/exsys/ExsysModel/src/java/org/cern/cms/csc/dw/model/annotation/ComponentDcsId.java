package org.cern.cms.csc.dw.model.annotation;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Class (fact) properties that are annotated with this annotation are told to be
 * carrying a component DCS ID (DCS datapoint element name), which is resolved by
 * org.cern.cms.csc.dw.ws.ComponentPropertyResolver through ontology component DCSId 
 * data property to two things - component and component attribute. The search algorithm
 * is looking for a component having a data property DCS_ID which *best* matches the
 * provided DCS ID, the unmatched part of the ID is considered to be a component attribute.
 * e.g. if you provide CSC_ME_M21_C01_HV.data.v1.status, it is matched to:
 *     component "ME-2/1/01/1/HVSegment1" and it's attribute "status".
 * @author evka
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface ComponentDcsId {

    /**
     * Component property name in the same class
     * @return
     */
    String componentProperty();

    /**
     * Component attribute property name in the same class.
     * @return
     */
    String componentAttributeProperty();

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
