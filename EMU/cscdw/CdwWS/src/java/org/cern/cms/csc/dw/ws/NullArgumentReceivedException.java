/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import org.apache.commons.lang.NullArgumentException;

/**
 *
 * @author valdo
 */
public class NullArgumentReceivedException extends NullArgumentException {

    /**
     * Constructs an instance of <code>NullArgumentReceivedException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public NullArgumentReceivedException(String argument) {
        super(argument);
    }
}
