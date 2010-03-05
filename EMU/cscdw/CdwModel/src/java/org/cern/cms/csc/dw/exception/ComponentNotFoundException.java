package org.cern.cms.csc.dw.exception;

/**
 *
 * @author valdo
 */
public class ComponentNotFoundException extends Exception {

    private final static String msg = "Component for id = [%s] not found in Knowledge Base";

    public ComponentNotFoundException(String componentId) {
        super(String.format(msg, componentId));
    }

}
