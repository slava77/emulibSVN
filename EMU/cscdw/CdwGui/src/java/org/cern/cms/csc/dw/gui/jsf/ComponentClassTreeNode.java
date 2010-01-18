/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.gui.jsf;

import com.icesoft.faces.component.tree.IceUserObject;
import javax.swing.tree.DefaultMutableTreeNode;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;

/**
 *
 * @author valdo
 */
public class ComponentClassTreeNode extends IceUserObject {

    private  ComponentClass componentClass = null;

    public ComponentClass getComponentClass() {
        return componentClass;
    }

    public ComponentClassTreeNode(DefaultMutableTreeNode wrapper) {
        super(wrapper);
    }

    public ComponentClassTreeNode(DefaultMutableTreeNode wrapper, ComponentClass componentClass) {
        super(wrapper);
        this.componentClass = componentClass;
        setLeafIcon("tree_document.gif");
        setBranchContractedIcon("tree_folder_closed.gif");
        setBranchExpandedIcon("tree_folder_open.gif");
    }

}
