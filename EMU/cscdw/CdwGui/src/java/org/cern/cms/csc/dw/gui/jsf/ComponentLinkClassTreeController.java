/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.gui.jsf;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import javax.ejb.EJB;
import javax.faces.event.ActionEvent;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.util.JsfBeanBase;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.model.ontology.ComponentLink;

/**
 *
 * @author valdo
 */
@EJB(name="OntologyDaoRef", beanInterface=OntologyDaoLocal.class)
public class ComponentLinkClassTreeController extends JsfBeanBase {

    private OntologyDaoLocal ontologyDao;

    private DefaultTreeModel model;

    public DefaultTreeModel getModel() {
        return model;
    }

    private ComponentLinkClass selectedLinkClass;

    public ComponentLinkClass getSelectedLinkClass() {
        return selectedLinkClass;
    }

    public ComponentLinkClassTreeController() throws Exception {
        ontologyDao = (OntologyDaoLocal) getEjb("OntologyDaoRef");
        init();
    }

    public void init() throws Exception {

        DefaultMutableTreeNode rootNode = new DefaultMutableTreeNode();
        ComponentLinkClassTreeNode root = new ComponentLinkClassTreeNode(rootNode);
        rootNode.setUserObject(root);
        root.setExpanded(true);

        List<DefaultMutableTreeNode> hList = new ArrayList<DefaultMutableTreeNode>();
        hList.add(rootNode);

        while (!hList.isEmpty()) {
            List<DefaultMutableTreeNode> hListToAdd = new ArrayList<DefaultMutableTreeNode>();
            for (Iterator<DefaultMutableTreeNode> hListIt = hList.iterator(); hListIt.hasNext();) {
                DefaultMutableTreeNode currentNode = hListIt.next();
                hListIt.remove();
                ComponentLinkClassTreeNode currentCctn = (ComponentLinkClassTreeNode) currentNode.getUserObject();
                List<ComponentLinkClass> tlist = ontologyDao.getComponentLinkClasses(currentCctn == null ? null : currentCctn.getComponentLinkClass());
                for (Iterator<ComponentLinkClass> tIt = tlist.iterator(); tIt.hasNext(); ) {
                    DefaultMutableTreeNode node = new DefaultMutableTreeNode();
                    ComponentLinkClassTreeNode child = new ComponentLinkClassTreeNode(node, tIt.next());
                    child.setLeaf(true);
                    node.setUserObject(child);
                    currentNode.add(node);
                    ((ComponentLinkClassTreeNode)currentNode.getUserObject()).setLeaf(false);
                    hListToAdd.add(node);
                }
            }
            hList.addAll(hListToAdd);
        }

        model = new DefaultTreeModel(rootNode);

    }

    public void componentLinkClassNodeSelectedAction(ActionEvent event) {
        String id = (String) getParameter("componentLinkClassId");
        if (id == null) {
            selectedLinkClass = null;
        } else {
            selectedLinkClass = ontologyDao.getComponentLinkClassById(id);
            if (selectedLinkClass != null) {
                componentLinks = ontologyDao.getComponentLinks(selectedLinkClass);
            }
        }
    }

    private List<ComponentLink> componentLinks = null;

    public List<ComponentLink> getComponents() {
        return componentLinks;
    }

    public boolean getComponentsSet() {
        return (componentLinks != null && componentLinks.size() > 0);
    }

    private ComponentLink selectedComponentLink = null;

    public ComponentLink getSelectedComponentLink() {
        return selectedComponentLink;
    }

    public void setSelectedComponentLinkId(String componentId) {
        this.selectedComponentLink = ontologyDao.getComponentLinkById(componentId);
    }

}
