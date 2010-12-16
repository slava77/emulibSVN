package org.cern.cms.csc.dw.exception;

public abstract class OntologyException extends Exception {

    public enum OntologySource {
        GRAPH_DATABASE,
        RELATIONAL_DATABASE
    }

    private final static String msg = " (%s)";

    public OntologyException(OntologySource source, String message) {
        super(message != null ? message.concat(String.format(msg, source.name())) : message);
    }

}
