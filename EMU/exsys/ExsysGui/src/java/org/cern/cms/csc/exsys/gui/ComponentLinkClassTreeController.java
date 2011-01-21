package org.cern.cms.csc.exsys.gui;

import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import java.util.ArrayList;
import java.util.Collection;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.event.ActionEvent;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;

@ManagedBean
@SessionScoped
public class ComponentLinkClassTreeController extends JsfBeanBase {

    private static Logger logger = SimpleLogger.getLogger(ComponentLinkClassTreeController.class);

    @EJB
    private GOntologyDaoLocal ontologyDao;

    private DefaultTreeModel model;

    public DefaultTreeModel getModel() {
        if (model == null) {

            DefaultMutableTreeNode rootNode = new DefaultMutableTreeNode();
            ComponentLinkClassTreeNode root = new ComponentLinkClassTreeNode(rootNode);
            rootNode.setUserObject(root);
            rootNode.setAllowsChildren(false);
            root.setExpanded(true);

            buildNode(rootNode);

            this.model = new DefaultTreeModel(rootNode);
        }

        return this.model;
    }

    private void buildNode(DefaultMutableTreeNode parent) {
        ComponentLinkClassTreeNode node = (ComponentLinkClassTreeNode) parent.getUserObject();

        // Constructing children list
        Collection<GComponentLinkClass> ccList = new ArrayList<GComponentLinkClass>();
        if (node.isRoot()) {
            for (GComponentLinkClass clc: ontologyDao.getGComponentLinkClasses()) {
                if (clc.getParent() == null) {
                    ccList.add(clc);
                }
            }
        } else {
            ccList.addAll(node.getComponentLinkClass().getChildren());
        }

        if (!ccList.isEmpty()) {

            parent.setAllowsChildren(true);
            node.setLeaf(false);

            // Creating and adding children
            for (GComponentLinkClass clc: ccList) {
                DefaultMutableTreeNode child = new DefaultMutableTreeNode();
                ComponentLinkClassTreeNode cnode = new ComponentLinkClassTreeNode(child, clc);
                child.setUserObject(cnode);
                parent.add(child);
                buildNode(child);
            }
        } else {
            parent.setAllowsChildren(false);
            node.setLeaf(true);
        }

    }

    private GComponentLinkClass selectedLinkClass;

    public GComponentLinkClass getSelectedLinkClass() {
        return selectedLinkClass;
    }

    public void componentLinkClassNodeSelectedAction(ActionEvent event) {
        String strId = (String) getParameter("componentLinkClassId");
        if (strId == null) {
            selectedLinkClass = null;
        } else {
            try {
                selectedLinkClass = ontologyDao.getGComponentLinkClass(Long.parseLong(strId));
            } catch (ComponentLinkClassNotFoundException ex) {
                logger.error(null, ex);
                selectedLinkClass = null;
            }
        }
    }

}
