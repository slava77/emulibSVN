package org.cern.cms.csc.dw.model.ontology.graph;

public interface GComponentSynonym extends GNode {

    void setName(String name);
    String getName();

    GComponent getComponent();
    void setComponent(GComponent component);

}
