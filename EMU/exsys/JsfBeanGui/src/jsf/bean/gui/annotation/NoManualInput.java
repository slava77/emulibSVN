/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.annotation;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * This annotation tells the GUI not to show this value at all.
 * Optionally, if createDefaultValue is set to true, GUI will attempt to create a new value using the default constructor and assign it to the property.
 * @author Evka
 */

@Retention(RetentionPolicy.RUNTIME)
public @interface NoManualInput {
    /**
     * If this is true then default value for this field will be created by an editor when creating a new instance of the entity
     */
    boolean createDefaultValue() default false;
}
