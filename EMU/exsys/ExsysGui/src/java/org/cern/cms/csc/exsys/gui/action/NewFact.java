/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui.action;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.convert.Converter;
import javax.faces.model.SelectItem;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.dao.EditorDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.metadata.MetadataManager;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.ws.FactCollectionInputLocal;
import org.cern.cms.csc.exsys.gui.editor.EntityEditorManager;
import org.cern.cms.csc.exsys.gui.editor.converter.LovConverter;

/**
 *
 * @author valdo
 */

@ManagedBean
@SessionScoped
public class NewFact extends EntityEditorManager {

    private final static Logger logger = ExsysLogger.getLogger(NewFact.class);

    private static MetadataManager factMdManager;
    private List<SelectItem> factClassesSI;
    private Converter factClassListConverter;
    private Class factType;

    @EJB
    private FactCollectionInputLocal factCollectionInput;

    @EJB
    private EditorDaoLocal EditorDao;

    private Component factSource;
    private List<SelectItem> factSourcesSI;
    private Converter factSourceConverter;

    @Override
    protected EntityBase createEntity() throws Exception {
        if (factType != null) {
            return (Fact) factType.newInstance();
        }
        return null;
    }

    @Override
    public String doSave() throws Exception {
        FactCollection fc = new FactCollection();
        List<FactCollectionFactsItem> factsItems = new ArrayList<FactCollectionFactsItem>();
        FactCollectionFactsItem fi = new FactCollectionFactsItem();
        fi.setItemValue(getFact());
        factsItems.add(fi);
        fc.setFactsItems(factsItems);
        fc.setTime(new Date());
        fc.setComponent(getFactSource());
        factCollectionInput.getFactCollection(fc);

        logger.info("component: " + getFact().getComponent().getName());

        return "navRules";
    }

    /**
     * Get the fact being edited/created.
     */
    public Fact getFact() {
        return (Fact) getEntity();
    }

    private static MetadataManager getFactMdManager() {
        if (factMdManager == null) {
            factMdManager = new MetadataManager();
        }
        return factMdManager;
    }

    public List<SelectItem> getFactClassList() {
        if (factClassesSI == null) {
            Collection<FactMd> factClassMds = getFactMdManager().getFactMDs();
            List<FactMd> factClassMdsSortedList = new ArrayList<FactMd>();
            factClassMdsSortedList.addAll(factClassMds);
            Collections.sort(factClassMdsSortedList);
            
            List<Class> factClasses = new ArrayList<Class>();
            factClassesSI = new ArrayList<SelectItem>();
            for (FactMd factClassMd: factClassMdsSortedList) {
                Class factClass = factClassMd.getFactClass();
                factClasses.add(factClass);
                factClassesSI.add(new SelectItem(factClass, factClass.getSimpleName()));
            }
            factClassListConverter = new LovConverter(factClasses);
        }
        return factClassesSI;
    }

    public Converter getFactClassListConverter() {
        return factClassListConverter;
    }

    public Class getFactType() {
        return factType;
    }

    public void setFactType(Class factType) {
        if ((factType != null) && !factType.equals(this.factType)) {
            this.factType = factType;
            reset();
        }
    }

    public Component getFactSource() {
        return factSource;
    }

    public void setFactSource(Component factSource) {
        this.factSource = factSource;
    }

    public List<SelectItem> getFactSourcesList() throws ComponentNotFoundException {
        if (factSourcesSI == null) {
            factSourcesSI = new ArrayList<SelectItem>();
            GComponentClass factSourceClass = getEditorDao().getGOntologyDao().getGComponentClass(ComponentClassType.FACT_PROVIDER);
            Collection<GComponent> factSourceGComponents = factSourceClass.getComponents();
            List<Component> factSources = new ArrayList<Component>();
            for (GComponent factSourceGComponent: factSourceGComponents) {
                Component source = getEditorDao().getOntologyDao().getComponent(factSourceGComponent);
                factSources.add(source);
                SelectItem si = new SelectItem(source, source.getName());
                factSourcesSI.add(si);
            }
            if (factSources.size() > 0) {
                setFactSource(factSources.get(0));
            }
            factSourceConverter = new LovConverter(factSources);
        }
        return factSourcesSI;
    }

    public Converter getFactSourceConverter() {
        return factSourceConverter;
    }

    @Override
    protected EditorDaoLocal getEditorDao() {
        return EditorDao;
    }

}
