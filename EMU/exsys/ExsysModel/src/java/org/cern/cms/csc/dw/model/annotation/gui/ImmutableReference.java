/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.annotation.gui;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Used on entity properties which point to other entities to tell the GUI editor
 * that it should not let the user to edit the referenced entity.
 * e.g. you may want to let a user to choose a component, but you don't want to let him edit it,
 * then you use this annotation on the property which is referencing a component.
 * @author Evka
 */

@Retention(RetentionPolicy.RUNTIME)
public @interface ImmutableReference {
}
