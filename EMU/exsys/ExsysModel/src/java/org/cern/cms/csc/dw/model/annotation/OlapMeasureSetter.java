package org.cern.cms.csc.dw.model.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface OlapMeasureSetter {

    /**
     * OLAP measure human readable name
     * @return
     */
    String name();

    /**
     * Dimension property name
     * @return
     */
    String propertyName();

    /**
     * Measure default aggregator type
     * @return
     */
    OlapMeasure.AggregatorType aggregator();

    /**
     * Measure output format
     * @return
     */
    String formatString() default "";

}
