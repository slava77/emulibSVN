/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.annotation;

/**
 * Class properties which have this annotation participate in building a title of an object of that class
 * @author Evka
 */
public @interface UseInTitle {
    int order();
}
