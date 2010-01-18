/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.gui.jsf;

import com.icesoft.faces.component.tree.IceUserObject;
import javax.swing.tree.DefaultMutableTreeNode;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;

/**
 *
 * @author valdo
 */
public class ComponentLinkClassTreeNode extends IceUserObject {

    private  ComponentLinkClass componentLinkClass = null;

    public ComponentLinkClass getComponentLinkClass() {
        return componentLinkClass;
    }

    public ComponentLinkClassTreeNode(DefaultMutableTreeNode wrapper) {
        super(wrapper);
    }

    public ComponentLinkClassTreeNode(DefaultMutableTreeNode wrapper, ComponentLinkClass componentLinkClass) {
        super(wrapper);
        this.componentLinkClass = componentLinkClass;
        setLeafIcon("tree_document.gif");
        setBranchContractedIcon("tree_folder_closed.gif");
        setBranchExpandedIcon("tree_folder_open.gif");
    }

}
