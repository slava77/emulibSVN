/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.annotation.gui;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Class properties which have this annotation participate in building a title of an object of that class
 * @author Evka
 */
@Retention(RetentionPolicy.RUNTIME)
public @interface Label {
    String name();
    String description();
}
