package org.cern.cms.csc.dw.exception;

/**
 *
 * @author valdo
 */
public class ComponentLinkClassNotFoundException extends OntologyException {

    private final static String dmsg = "Component Link Class for id = [%d] not found in Knowledge Base";

    public ComponentLinkClassNotFoundException(OntologySource source, Long id) {
        super(source, String.format(dmsg, id));
    }

}
