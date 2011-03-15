/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

import java.util.Properties;

/**
 *
 * @author evka
 */
public class ExsysIORemoteEjbLookup<T> extends EjbLookup<T> {

    private static final String exsysIOHostname;
    private static final int exsysIOIIOPPort;

    static {
        exsysIOHostname = PropertiesProvider.getProperty("exsysIO.hostname");
        exsysIOIIOPPort = Integer.valueOf(PropertiesProvider.getProperty("exsysIO.IIOP.port"));
    }

    public ExsysIORemoteEjbLookup(Class<T> ejbRemoteInterface) {
        super(ejbRemoteInterface, exsysIOHostname, exsysIOIIOPPort);
    }
}
