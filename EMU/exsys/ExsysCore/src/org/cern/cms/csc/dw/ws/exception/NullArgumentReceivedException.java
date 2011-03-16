/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws.exception;

/**
 *
 * @author valdo
 */
public class NullArgumentReceivedException extends Exception {

    /**
     * Constructs an instance of <code>NullArgumentReceivedException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public NullArgumentReceivedException(String argument) {
        super(argument);
    }
}
