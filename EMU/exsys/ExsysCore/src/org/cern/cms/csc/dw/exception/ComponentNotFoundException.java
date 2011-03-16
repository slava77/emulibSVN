package org.cern.cms.csc.dw.exception;

import org.cern.cms.csc.dw.model.ontology.graph.GNode.PropertyType;

public class ComponentNotFoundException extends OntologyException {

    private final static String smsg = "Component for name = [%s] not found in Knowledge Base";
    private final static String dmsg = "Component for id = [%d] not found in Knowledge Base";
    private final static String propmsg = "Component for property [%s] = [%s] not found in Knowledge Base";

    public ComponentNotFoundException(OntologySource source, Long id) {
        super(source, String.format(dmsg, id));
    }

    public ComponentNotFoundException(OntologySource source, String name) {
        super(source, String.format(smsg, name));
    }

    public ComponentNotFoundException(OntologySource source, PropertyType propType, Object propValue) {
        super(source, String.format(propmsg, propType.propertyName(), propValue.toString()));
    }

}
