package org.cern.cms.csc.dw.model.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface OlapDimensionSetter {

    /**
     * Dimension name
     * @return
     */
    String name();

    /**
     * Dimension property name
     * @return
     */
    String propertyName();

    /**
     * Name of the shared dimension table name (classifier table name)
     * @return
     */
    String sharedTable() default "";

    /**
     * If this field is a base field for the shared dimension?
     * i.e. shared dimension is being constructed out of this table + field
     * @return
     */
    boolean baseField() default false;

}
