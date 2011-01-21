package org.cern.cms.csc.exsys.gui;

import com.icesoft.faces.component.tree.IceUserObject;
import javax.swing.tree.DefaultMutableTreeNode;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;

public class ComponentLinkClassTreeNode extends IceUserObject {

    private  GComponentLinkClass componentLinkClass = null;

    public GComponentLinkClass getComponentLinkClass() {
        return componentLinkClass;
    }

    public boolean isRoot() {
        return componentLinkClass == null;
    }

    public ComponentLinkClassTreeNode(DefaultMutableTreeNode wrapper) {
        super(wrapper);
    }

    public ComponentLinkClassTreeNode(DefaultMutableTreeNode wrapper, GComponentLinkClass componentLinkClass) {
        super(wrapper);
        this.componentLinkClass = componentLinkClass;
        setLeafIcon("tree_document.gif");
        setBranchContractedIcon("tree_folder_closed.gif");
        setBranchExpandedIcon("tree_folder_open.gif");
    }

}
