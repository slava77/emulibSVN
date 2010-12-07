package org.cern.cms.csc.dw.gui.jsf;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import javax.ejb.EJB;
import javax.faces.event.ActionEvent;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.util.ServiceLocator;

@EJB(name="OntologyDaoRef", beanInterface=OntologyDaoLocal.class)
public class ComponentLinkClassTreeController extends JsfBeanBase {

    private static Logger logger = Logger.getLogger(ComponentLinkClassTreeController.class);

    private OntologyDaoLocal ontologyDao;

    private DefaultTreeModel model;

    public DefaultTreeModel getModel() {
        return model;
    }

    private GComponentLinkClass selectedLinkClass;

    public GComponentLinkClass getSelectedLinkClass() {
        return selectedLinkClass;
    }

    public ComponentLinkClassTreeController() throws Exception {
        ontologyDao = (OntologyDaoLocal) ServiceLocator.getInstance().getEnvService("OntologyDaoRef");
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
                Collection<GComponentLinkClass> tlist;
                if (currentCctn == null || currentCctn.getComponentLinkClass() == null) {
                    tlist = ontologyDao.getGComponentLinkClasses();
                    Collection<GComponentLinkClass> toRemove = new HashSet<GComponentLinkClass>();
                    for (GComponentLinkClass c: tlist) {
                        if (c.getParent() != null) {
                            toRemove.add(c);
                        }
                    }
                    tlist.removeAll(toRemove);
                } else {
                    tlist = currentCctn.getComponentLinkClass().getChildren();
                }
                for (GComponentLinkClass c: tlist) {
                    DefaultMutableTreeNode node = new DefaultMutableTreeNode();
                    ComponentLinkClassTreeNode child = new ComponentLinkClassTreeNode(node, c);
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
