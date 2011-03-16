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
public class ExsysRemoteEjbLookup<T> extends EjbLookup<T> {

    private static final String exsysHostname;
    private static final int exsysIIOPPort;

    static {
        exsysHostname = PropertiesProvider.getProperty("exsys.hostname");
        exsysIIOPPort = Integer.valueOf(PropertiesProvider.getProperty("exsys.IIOP.port"));
    }

    public ExsysRemoteEjbLookup(Class<T> ejbRemoteInterface) {
        super(ejbRemoteInterface, exsysHostname, exsysIIOPPort);
    }
}
