/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.exception;

/**
 *
 * @author evka
 */
public class ActionExecutionException extends Exception {

    public ActionExecutionException(String message, Throwable cause) {
        super(message, cause);
    }

    public ActionExecutionException(String message) {
        super(message);
    }

}
