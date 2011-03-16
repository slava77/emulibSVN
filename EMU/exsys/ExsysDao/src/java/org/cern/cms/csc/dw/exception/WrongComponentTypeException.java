package org.cern.cms.csc.dw.exception;

import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;

public class WrongComponentTypeException extends Exception {

    private final static String msg = "Component [%s] of type [%s] is not allowed in [%s] class property [%s].";

    private static String getMessage(String componentName, String componentType, Class clazz, String propertyName) {
        return String.format(msg, componentName, componentType, clazz.getName(), propertyName);
    }

    public WrongComponentTypeException(GComponent gc, Class clazz, String propertyName) {
        super(getMessage(gc.getName(), gc.getType().getType().getValue(), clazz, propertyName));
    }

    public WrongComponentTypeException(Component c, Class clazz, String propertyName) {
        super(getMessage(c.getName(), c.getType().getType().getValue(), clazz, propertyName));
    }

}
