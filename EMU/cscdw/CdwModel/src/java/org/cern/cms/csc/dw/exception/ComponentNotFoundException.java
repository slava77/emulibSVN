package org.cern.cms.csc.dw.exception;

public class ComponentNotFoundException extends OntologyException {

    private final static String smsg = "Component for name = [%s] not found in Knowledge Base";
    private final static String dmsg = "Component for id = [%d] not found in Knowledge Base";

    public ComponentNotFoundException(OntologySource source, Long id) {
        super(source, String.format(dmsg, id));
    }

    public ComponentNotFoundException(OntologySource source, String name) {
        super(source, String.format(smsg, name));
    }

}
