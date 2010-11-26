/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.annotation.gui;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 *
 * @author Evka
 */

@Retention(RetentionPolicy.RUNTIME)
public @interface NoManualInput {
    /**
     * If this is true then default value for this field will be created by an editor when creating a new instance of the entity
     */
    boolean createDefaultValue() default false;
}
