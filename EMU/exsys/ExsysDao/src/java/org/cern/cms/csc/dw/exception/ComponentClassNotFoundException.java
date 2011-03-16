package org.cern.cms.csc.dw.exception;

public class ComponentClassNotFoundException extends OntologyException {

    private final static String dmsg = "Component Class for id = [%d] not found in Knowledge Base";

    public ComponentClassNotFoundException(OntologySource source, Long id) {
        super(source, String.format(dmsg, id));
    }

}
