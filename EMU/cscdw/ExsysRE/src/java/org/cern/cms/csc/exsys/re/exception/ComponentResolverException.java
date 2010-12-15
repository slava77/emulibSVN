/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.exception;

/**
 *
 * @author evka
 */
public class ComponentResolverException extends Exception {

    public ComponentResolverException(String message, Throwable cause) {
        super(message, cause);
    }

    public ComponentResolverException(String message) {
        super(message);
    }

}
