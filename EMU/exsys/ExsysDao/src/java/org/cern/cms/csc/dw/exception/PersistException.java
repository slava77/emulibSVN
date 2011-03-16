/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.exception;

/**
 *
 * @author Evka
 */
public class PersistException extends Exception {

    public PersistException(Throwable cause) {
        super(cause);
    }

    public PersistException(String msg) {
        super(msg);
    }

    public PersistException(String msg, Throwable cause) {
        super(msg, cause);
    }
}
