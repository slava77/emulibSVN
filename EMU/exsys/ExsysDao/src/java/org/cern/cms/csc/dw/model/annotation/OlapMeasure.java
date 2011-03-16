package org.cern.cms.csc.dw.model.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface OlapMeasure {

    public enum AggregatorType {

        SUM("sum"),
        COUNT("count"),
        DISTINCT_COUNT("distinct-count"),
        MIN("min"),
        MAX("max"),
        AVG("avg");

        private final String value;

        AggregatorType(String v) {
            value = v;
        }

        public String getValue() {
            return value;
        }

    }

    /**
     * OLAP measure human readable name
     * @return
     */
    String name();

    /**
     * Measure default aggregator type
     * @return
     */
    AggregatorType aggregator();

    /**
     * Measure output format
     * @return
     */
    String formatString() default "";

}
