/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.exception;

/**
 *
 * @author evka
 */
public class DimException extends Exception {

    public DimException(String message, Throwable cause) {
        super(message, cause);
    }

    public DimException(String message) {
        super(message);
    }

}
