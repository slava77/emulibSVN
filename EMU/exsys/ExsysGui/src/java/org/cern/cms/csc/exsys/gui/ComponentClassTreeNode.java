package org.cern.cms.csc.exsys.gui;

import com.icesoft.faces.component.tree.IceUserObject;
import javax.swing.tree.DefaultMutableTreeNode;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;

public class ComponentClassTreeNode extends IceUserObject {

    private GComponentClass componentClass = null;

    public GComponentClass getComponentClass() {
        return componentClass;
    }

    public boolean isRoot() {
        return componentClass == null;
    }

    public ComponentClassTreeNode(DefaultMutableTreeNode wrapper) {
        super(wrapper);
    }

    public ComponentClassTreeNode(DefaultMutableTreeNode wrapper, GComponentClass componentClass) {
        super(wrapper);
        this.componentClass = componentClass;
        setLeafIcon("tree_document.gif");
        setBranchContractedIcon("tree_folder_closed.gif");
        setBranchExpandedIcon("tree_folder_open.gif");
    }

}
