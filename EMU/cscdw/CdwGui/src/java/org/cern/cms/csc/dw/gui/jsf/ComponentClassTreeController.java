package org.cern.cms.csc.dw.gui.jsf;

import com.icesoft.faces.component.selectinputtext.SelectInputText;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.TreeSet;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import javax.faces.model.SelectItem;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.util.ServiceLocator;

@EJB(name="OntologyDaoRef", beanInterface=OntologyDaoLocal.class)
public class ComponentClassTreeController extends JsfBeanBase {

    private OntologyDaoLocal ontologyDao;  
    private DefaultTreeModel model;

    private GComponentClass selectedComponentClass = null;
    private Collection<GComponentClass> componentMatchClasses = Collections.EMPTY_SET;
    private GComponent selectedComponent = null;
    private String selectedComponentName = null;
    private List<SelectItem> componentMatches;
    private static final int componentMatchesToDisplay = 15;

    public DefaultTreeModel getModel() {
        return model;
    }

    public GComponentClass getSelectedComponentClass() {
        return selectedComponentClass;
    }

    public ComponentClassTreeController() throws Exception {
        ontologyDao = (OntologyDaoLocal) ServiceLocator.getInstance().getEnvService("OntologyDaoRef");
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
                Collection<GComponentClass> tlist = null;
                if (currentCctn == null || currentCctn.getComponentClass() == null) {
                    tlist = ontologyDao.getGComponentClasses();
                    Collection<GComponentClass> toRemove = new HashSet<GComponentClass>();
                    for (GComponentClass c: tlist) {
                        if (!c.getParents().isEmpty()) {
                            toRemove.add(c);
                        }
                    }
                    tlist.removeAll(toRemove);
                } else {
                    tlist = currentCctn.getComponentClass().getChildren();
                }
                for (GComponentClass c: tlist) {
                    DefaultMutableTreeNode node = new DefaultMutableTreeNode();
                    ComponentClassTreeNode child = new ComponentClassTreeNode(node, c);
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
            selectedComponentClass = null;
            componentMatchClasses = Collections.EMPTY_SET;
        } else {
            try {
                selectedComponentClass = ontologyDao.getGComponentClass(Long.parseLong(strId));
                componentMatchClasses = selectedComponentClass.getChildrenRecursive(true);
            } catch (ComponentClassNotFoundException ex) {
                Logger.getLogger(ComponentClassTreeController.class.getName()).log(Level.SEVERE, null, ex);
                selectedComponentClass = null;
                componentMatchClasses = Collections.EMPTY_SET;
            }
        }
    }

    public GComponent getSelectedComponent() {
        return selectedComponent;
    }

    public void setSelectedComponent(GComponent selectedComponent) {
        this.selectedComponent = selectedComponent;
    }

    public void setSelectedComponentName(String selectedComponentName) {
        this.selectedComponentName = selectedComponentName;
    }

    public String getSelectedComponentName() {
        return selectedComponentName;
    }

    public void setSelectedComponentId(Long componentId) throws ComponentNotFoundException {
        this.selectedComponent = ontologyDao.getGComponent(componentId);
    }

    public void resetAction(ActionEvent event) {
        selectedComponentClass = null;
        selectedComponent = null;
    }

    public void componentInputValueChanged(ValueChangeEvent event) {

        if (event.getComponent() instanceof SelectInputText) {

            // get the number of displayable records from the component
            SelectInputText autoComplete = (SelectInputText) event.getComponent();
            // get the new value typed by component user.
            String newWord = (String) event.getNewValue();

            componentMatches = new ArrayList<SelectItem>();
            selectedComponent = null;

            if (newWord != null && newWord.length() > 0) {

                for (GComponent gc: ontologyDao.getGComponentsByNameMatches(newWord, componentMatchClasses, getComponentMatchesToDisplay())) {
                    componentMatches.add(new SelectItem(gc, gc.getName()));
                }

                // if there is a selected item then find the object of the
                // same name
                if (autoComplete.getSelectedItem() != null) {
                    selectedComponent = (GComponent) autoComplete.getSelectedItem().getValue();
                }

                // if there was no selection we still want to see if a proper
                // object was typed and update our selectedObject instance.
                else {
                    if (componentMatches != null) {
                        GComponent tmp;
                        for(int i = 0, max = componentMatches.size(); i < max; i++){
                            tmp = (GComponent) componentMatches.get(i).getValue();
                            if (tmp.getName().compareToIgnoreCase(newWord) == 0) {
                                selectedComponent = tmp;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    public List<SelectItem> getComponentMatches() {
        return componentMatches;
    }

    public int getComponentMatchesToDisplay() {
        return componentMatchesToDisplay;
    }

}
