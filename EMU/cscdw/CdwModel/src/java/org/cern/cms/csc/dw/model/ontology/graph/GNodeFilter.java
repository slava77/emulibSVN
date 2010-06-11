package org.cern.cms.csc.dw.model.ontology.graph;

/**
 * Interface that is used to filter nodes on demand
 * One should implement filter(GNode) method to provide a custom filter.
 */
public interface GNodeFilter {

    public boolean filter(GNode node);

}
