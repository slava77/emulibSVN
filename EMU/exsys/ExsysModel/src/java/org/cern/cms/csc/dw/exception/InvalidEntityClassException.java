/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.exception;

/**
 *
 * @author Evka
 */
public class InvalidEntityClassException extends Exception {

    public InvalidEntityClassException(Throwable cause) {
        super(cause);
    }

    public InvalidEntityClassException(String className) {
        super("Unknown class: " + className);
    }

    public InvalidEntityClassException(String className, String detailMsg) {
        super("Unknown class: " + className + ". Details: " + detailMsg);
    }
}
