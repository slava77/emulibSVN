package org.cern.cms.csc.dw.exception;

import org.cern.cms.csc.dw.model.ontology.ComponentClassType;

/**
 *
 * @author valdo
 */
public class ComponentTypeNotAllowedInFactException extends Exception {

    private final static String msg = "Component with type [%s] is not allowed in fact type [%s].";

    public ComponentTypeNotAllowedInFactException(ComponentClassType cct, Class factClass) {
        super(String.format(msg, cct.value(), factClass.getName()));
    }

}
