package org.cern.cms.csc.dw.model.ontology.graph;

public abstract class GBase {

    protected GServices gservices;

    protected GBase(GServices gservices) {
        this.gservices = gservices;
    }

    public static Class<? extends GNode> ifClass(Class implClass) {
        
        if (implClass.equals(GComponentImpl.class))
            return GComponent.class;
        
        if (implClass.equals(GComponentClassImpl.class))
            return GComponentClass.class;
        
        if (implClass.equals(GComponentLinkClassImpl.class))
            return GComponentLinkClass.class;
        
        if (implClass.equals(GComponentLinksImpl.class))
            return GComponentLinks.class;
        
        if (implClass.equals(GComponentSynonymImpl.class))
            return GComponentSynonym.class;

        return null;

    }

    public static Class<? extends GNodeImpl> implClass(Class ifClass) {

        if (ifClass.equals(GComponent.class))
            return GComponentImpl.class;

        if (ifClass.equals(GComponentClass.class))
            return GComponentClassImpl.class;

        if (ifClass.equals(GComponentLinkClass.class))
            return GComponentLinkClassImpl.class;

        if (ifClass.equals(GComponentLinks.class))
            return GComponentLinksImpl.class;

        if (ifClass.equals(GComponentSynonym.class))
            return GComponentSynonymImpl.class;

        return null;
        
    }

}
