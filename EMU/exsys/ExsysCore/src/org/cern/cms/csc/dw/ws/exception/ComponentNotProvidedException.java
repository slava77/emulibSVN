package org.cern.cms.csc.dw.ws.exception;

public class ComponentNotProvidedException extends Exception {

    public ComponentNotProvidedException(Class clazz, String property) {
        super(clazz.getName() + " " + property + " not provided!");
    }

}
