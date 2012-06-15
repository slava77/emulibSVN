/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Defines how to display entity attribute. Possible combinations:
 * 
 * - type = Type.ENTITY : entity title is retrieved from getEntityTitle method in EntityBeanBase, value is not used
 * - type = Type.CONSTANT : entity title is a constant string defined in value attribute
 * - type = Type.PROPERTY : entity title is the entity holding bean property value defined in value
 * 
 * @author Valdo
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.FIELD)
public @interface EntityTitle {
    
    EntityTitleType type() default EntityTitleType.ENTITY;
    String value() default "";
    
}
