package org.cern.cms.csc.exsys.gui;

import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.event.ActionEvent;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.exsys.gui.component.OntologyComponentSelector;

@ManagedBean
@SessionScoped
public class ComponentClassTreeController extends JsfBeanBase {

    private static Logger logger = SimpleLogger.getLogger(ComponentClassTreeController.class);

    @EJB
    private GOntologyDaoLocal gOntologyDao;

    private DefaultTreeModel model = null;
    private GComponentClass selectedComponentClass = null;
    private OntologyComponentSelector componentSelector;

    public ComponentClassTreeController() {
        this.componentSelector = new OntologyComponentSelector() {
            @Override
            protected GOntologyDaoLocal getGOntologyDao() {
                return gOntologyDao;
            }
        };
    }

    public DefaultTreeModel getModel() {
        if (model == null) {

            DefaultMutableTreeNode rootNode = new DefaultMutableTreeNode();
            ComponentClassTreeNode root = new ComponentClassTreeNode(rootNode);
            rootNode.setUserObject(root);
            rootNode.setAllowsChildren(false);
            root.setExpanded(true);

            buildNode(rootNode);

            this.model = new DefaultTreeModel(rootNode);
        }
        
        return this.model;
    }

    private void buildNode(DefaultMutableTreeNode parent) {
        ComponentClassTreeNode node = (ComponentClassTreeNode) parent.getUserObject();

        // Constructing children list
        Collection<GComponentClass> ccList = new ArrayList<GComponentClass>();
        if (node.isRoot()) {
            for (GComponentClass cc: gOntologyDao.getGComponentClasses()) {
                if (cc.getParents().isEmpty()) {
                    ccList.add(cc);
                }
            }
        } else {
            ccList.addAll(node.getComponentClass().getChildren());
        }

        if (!ccList.isEmpty()) {

            parent.setAllowsChildren(true);
            node.setLeaf(false);

            // Creating and adding children
            for (GComponentClass cc: ccList) {
                DefaultMutableTreeNode child = new DefaultMutableTreeNode();
                ComponentClassTreeNode cnode = new ComponentClassTreeNode(child, cc);
                child.setUserObject(cnode);
                parent.add(child);
                buildNode(child);
            }
        } else {
            parent.setAllowsChildren(false);
            node.setLeaf(true);
        }

    }

    public GComponentClass getSelectedComponentClass() {
        return selectedComponentClass;
    }

    public void componentClassNodeSelectedAction(ActionEvent event) {
        String strId = (String) getParameter("componentClassId");
        if (strId == null) {
            selectedComponentClass = null;
            componentSelector.setLimitTo(Collections.EMPTY_SET);
        } else {
            try {
                selectedComponentClass = gOntologyDao.getGComponentClass(Long.parseLong(strId));
                componentSelector.setLimitTo(selectedComponentClass.getChildrenRecursive(true));
            } catch (ComponentClassNotFoundException ex) {
                logger.error(null, ex);
                selectedComponentClass = null;
                componentSelector.setLimitTo(Collections.EMPTY_SET);
            }
        }
    }

    public OntologyComponentSelector getComponentSelector() {
        return componentSelector;
    }

    public void resetAction(ActionEvent event) {
        selectedComponentClass = null;
        componentSelector.setLimitTo(Collections.EMPTY_SET);
        componentSelector.setComponent(null);
    }

}
