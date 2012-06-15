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
 * This annotation tells the GUI to create a default value for this property.
 * GUI will attempt to create a new value using the default constructor and assign it to the property.
 * @author Evka
 */

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.FIELD)
public @interface CreateDefaultValue {
    /**
     * If you clazz == Object.class then the default constructor is attempted to be called on the same class as the property,
     * if clazz is something else then the default constructor is attempted to be called on the given class.
     */
    Class clazz() default Object.class;
}
