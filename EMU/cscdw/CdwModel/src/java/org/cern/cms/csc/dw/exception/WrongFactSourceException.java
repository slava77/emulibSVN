package org.cern.cms.csc.dw.exception;

import org.cern.cms.csc.dw.model.ontology.graph.GComponent;

public class WrongFactSourceException extends Exception {

    private final static String msg = "Component [%s] of type [%s] is not of FactProvider class.";

    public WrongFactSourceException(GComponent gc) {
        super(String.format(msg, gc.getName(), gc.getType().getType().getValue()));
    }

}
