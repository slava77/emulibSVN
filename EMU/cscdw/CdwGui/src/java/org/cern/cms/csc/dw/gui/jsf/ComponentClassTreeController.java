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
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.util.JsfBeanBase;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.model.ontology.Component;

/**
 *
 * @author valdo
 */
@EJB(name="OntologyDaoRef", beanInterface=OntologyDaoLocal.class)
public class ComponentClassTreeController extends JsfBeanBase {

    private OntologyDaoLocal ontologyDao;

    private DefaultTreeModel model;

    public DefaultTreeModel getModel() {
        return model;
    }

    private ComponentClass selectedClass;

    public ComponentClass getSelectedClass() {
        return selectedClass;
    }

    public ComponentClassTreeController() throws Exception {
        ontologyDao = (OntologyDaoLocal) getEjb("OntologyDaoRef");
        init();
    }

    public void init() throws Exception {

        DefaultMutableTreeNode rootNode = new DefaultMutableTreeNode();
        ComponentClassTreeNode root = new ComponentClassTreeNode(rootNode);
        rootNode.setUserObject(root);
        root.setExpanded(true);

        List<DefaultMutableTreeNode> hList = new ArrayList<DefaultMutableTreeNode>();
        hList.add(rootNode);

        while (!hList.isEmpty()) {
            List<DefaultMutableTreeNode> hListToAdd = new ArrayList<DefaultMutableTreeNode>();
            for (Iterator<DefaultMutableTreeNode> hListIt = hList.iterator(); hListIt.hasNext();) {
                DefaultMutableTreeNode currentNode = hListIt.next();
                hListIt.remove();
                ComponentClassTreeNode currentCctn = (ComponentClassTreeNode) currentNode.getUserObject();
                List<ComponentClass> tlist = ontologyDao.getComponentClasses(currentCctn == null ? null : currentCctn.getComponentClass());
                for (Iterator<ComponentClass> tIt = tlist.iterator(); tIt.hasNext(); ) {
                    DefaultMutableTreeNode node = new DefaultMutableTreeNode();
                    ComponentClassTreeNode child = new ComponentClassTreeNode(node, tIt.next());
                    child.setLeaf(true);
                    node.setUserObject(child);
                    currentNode.add(node);
                    ((ComponentClassTreeNode)currentNode.getUserObject()).setLeaf(false);
                    hListToAdd.add(node);
                }
            }
            hList.addAll(hListToAdd);
        }

        model = new DefaultTreeModel(rootNode);

    }

    public void componentClassNodeSelectedAction(ActionEvent event) {
        String strId = (String) getParameter("componentClassId");
        if (strId == null) {
            selectedClass = null;
        } else {
            selectedClass = ontologyDao.getComponentClassById(Long.parseLong(strId));
            if (selectedClass != null) {
                components = ontologyDao.getComponents(selectedClass);
            }
        }
    }

    private List<Component> components = null;

    public List<Component> getComponents() {
        return components;
    }

    public boolean getComponentsSet() {
        return (components != null && components.size() > 0);
    }

    private Component selectedComponent = null;

    public Component getSelectedComponent() {
        return selectedComponent;
    }

    public void setSelectedComponentId(Long componentId) throws ComponentNotFoundException {
        this.selectedComponent = ontologyDao.getComponentById(componentId, true);
    }

}
