/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.cern.cms.csc.exsys.gui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import javax.faces.convert.Converter;
import javax.faces.model.SelectItem;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NameClassPair;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import javax.persistence.criteria.CriteriaBuilder;
import javax.persistence.criteria.CriteriaQuery;
import javax.persistence.criteria.Path;
import javax.persistence.criteria.Predicate;
import javax.persistence.criteria.Root;
import javax.persistence.criteria.Subquery;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.metadata.MetadataManager;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem;
import org.cern.cms.csc.dw.service.ServiceInstructions;
import org.cern.cms.csc.dw.ws.FactCollectionSaverLocal;
import org.cern.cms.csc.exsys.gui.editor.converter.LovConverter;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;

/**
 *
 * @author evka
 */
@ManagedBean
@RequestScoped
public class FactReplay {

    private static final Logger logger = ExsysLogger.getLogger(FactReplay.class);
    
    private static final String DATASOURCE_JNDI_CONTEXT = "jdbc";
    private static final int MAX_COLLECTION_SIZE = 500;
    
    private List<Class<? extends Fact>> allFactClasses;
    private Converter factClassConverter;
    private String datasourceName;
    private EntityManager em;
    private List<Class<? extends Fact>> factTypes = new ArrayList<Class<? extends Fact>>();
    private Date startTime = new Date();
    private Date endTime = new Date();
    
    @EJB
    private FactCollectionSaverLocal factCollectionSaver;
    
    @EJB
    private RuleEngineManagerLocal reManager;
    
    /** Creates a new instance of FactReplay */
    public FactReplay() {
    }

    public List<Class<? extends Fact>> getFactTypes() {
        return factTypes;
    }

    public void setFactTypes(List<Class<? extends Fact>> factTypes) {
        this.factTypes = factTypes;
    }
    
    public List<SelectItem> getAllFactTypes() {
        List<SelectItem> ret = new ArrayList<SelectItem>();
        for (Class clazz: getAllFactClasses()) {
            ret.add(new SelectItem(clazz, clazz.getSimpleName()));
        }
        return ret;
    }

    public Converter getFactClassConverter() {
        if (factClassConverter == null) {
            factClassConverter = new LovConverter(getAllFactClasses());
        }
        return factClassConverter;
    }
    
    private List<Class<? extends Fact>> getAllFactClasses() {
        if (allFactClasses == null) {
            MetadataManager mm = new MetadataManager();
            Collection<FactMd> factMds = mm.getFactMDs();
            allFactClasses = new ArrayList<Class<? extends Fact>>();
            for (FactMd factMd: factMds) {
                allFactClasses.add(factMd.getFactClass());
            }
            Comparator<Class<? extends Fact>> comp = new Comparator<Class<? extends Fact>>() {

                @Override
                public int compare(Class<? extends Fact> f1, Class<? extends Fact> f2) {
                    return f1.getSimpleName().compareToIgnoreCase(f2.getSimpleName());
                }
            };
            Collections.sort(allFactClasses, comp);
        }
        return allFactClasses;
    }
    
    public void go() throws Exception {
        Date t1 = new Date();
        
        logger.info("Fact Replay: starting replaying facts from \"" + getDatasourceName() + "\" of these types:");
        for (Class clazz: getFactTypes()) {
            logger.info("    * " + clazz.getSimpleName());
        }
        
        if (getFactTypes().isEmpty()) {
            return;
        }
        
        logger.info("Fact Replay: Fetching facts from the datasource...");

        // subquery approach
//        CriteriaBuilder builder = getEntityManager().getCriteriaBuilder();
//        CriteriaQuery baseFactCriteria = builder.createQuery(Fact.class);
//        Root<Fact> fromBaseFact = baseFactCriteria.from(Fact.class);
//        //fromBaseFact.fetch("component");
//        CriteriaQuery selectBaseFact = baseFactCriteria.select(fromBaseFact);
//        Path baseFactIdPath = fromBaseFact.get("id");
//        
//        List<Predicate> idInPredicates = new ArrayList<Predicate>();
//        for (Class<? extends Fact> factClass: getFactTypes()) {
//            Subquery subquery = baseFactCriteria.subquery(factClass);
//            Root fromFact = subquery.from(factClass);
//            subquery.select(fromFact.get("id"));
//            subquery.where(builder.between(fromFact.get("time"), getStartTime(), getEndTime()));
//            idInPredicates.add(builder.in(baseFactIdPath).value(subquery));
//        }
//        selectBaseFact.where(builder.or(idInPredicates.toArray(new Predicate[] {})));
//        selectBaseFact.orderBy(builder.asc(fromBaseFact.get("time")));
//        List<Fact> facts = getEntityManager().createQuery(baseFactCriteria).getResultList();
        

        // fetch facts separately and sort them locally
        List<Fact> facts = new ArrayList<Fact>();
        for (Class<? extends Fact> factClass: getFactTypes()) {
            CriteriaBuilder builder = getEntityManager().getCriteriaBuilder();
            CriteriaQuery query = builder.createQuery(factClass);
            Root from = query.from(factClass);
            from.fetch("component");
            query.select(from);
            query.where(builder.between(from.get("time"), getStartTime(), getEndTime()));
            //query.orderBy(builder.asc(from.get("time")));
            facts.addAll(getEntityManager().createQuery(query).getResultList());
        }
        
        Comparator<Fact> timeComparator = new Comparator<Fact>() {

            @Override
            public int compare(Fact f1, Fact f2) {
                return f1.getTime().compareTo(f2.getTime());
            }
            
        };
        Collections.sort(facts, timeComparator);
        
        
        logger.info("Fact Replay: fetched " + facts.size() + " facts.");
        logger.info("Fact Replay: constructing a fact collection...");
        
        List<FactCollection> collections = new ArrayList<FactCollection>();
        FactCollection collection = null;
        List<FactCollectionFactsItem> factCollectionItems = null;
        for (int i=0; i < facts.size(); i++) {
            if ((i % MAX_COLLECTION_SIZE == 0) || (i == facts.size() - 1)) { // flush current collection
                if (collection != null) {
                    collection.setFactsItems(factCollectionItems);
                    collections.add(collection);
                }
                
                collection = new FactCollection();
                collection.setSource("DCSFactService");
                collection.setTime(new Date());
                ServiceInstructions si = new ServiceInstructions();
                si.setAsync(true);
                si.setStrict(true);
                si.setPersist(true);
                collection.setServiceInstructions(si);
                factCollectionItems = new ArrayList<FactCollectionFactsItem>();
            }

            Fact fact = facts.get(i);
            fact.setComponentId(fact.getComponent().getName());
            fact.setComponent(null);
            fact.setid(null);
            FactCollectionFactsItem fi = new FactCollectionFactsItem();
            fi.setItemValue(fact);
            factCollectionItems.add(fi);
        }
        
        logger.info("Fact Replay: Reconfiguring rules engine (external time)...");
        
        reManager.reconfigureForFactReplay();

        logger.info("Fact Replay: Sending " + collections.size() + " fact collections to factCollectionSaver...");

        for (FactCollection c: collections) {
            factCollectionSaver.saveFactCollection(c);
        }

        //logger.info("Fact Replay: Reconfiguring rules engine (internal time)...");
        
        //reManager.reconfigure();
        
        Date t2 = new Date();
        
        logger.info("Fact Replay: DONE, took " + (t2.getTime() - t1.getTime()) + "ms");
        logger.info("Fact Replay: Cosing the entity manager");

        closeEntityManager();
    }
    
    public List<SelectItem> getAllDatasources() throws NamingException {
        Context context = new InitialContext();
        NamingEnumeration<NameClassPair> names = context.list(DATASOURCE_JNDI_CONTEXT);
        List<SelectItem> ret = new ArrayList<SelectItem>();
        while (names.hasMore()) {
            ret.add(new SelectItem(DATASOURCE_JNDI_CONTEXT + "/" + names.next().getName()));
        }
        return ret;
    }
    
    private EntityManager getEntityManager() {
        if ((em == null) && (datasourceName != null)) {
            logger.debug("Creating entity manager for datasource " + datasourceName);
            Map<String, Object> overridenProps = new HashMap<String, Object>();
            overridenProps.put("javax.persistence.jtaDataSource", datasourceName);
            overridenProps.put("javax.persistence.transactionType", "RESOURCE_LOCAL");
            EntityManagerFactory emf = Persistence.createEntityManagerFactory("CdwPU", overridenProps);
            em = emf.createEntityManager();
        }
        return em;
    }
    
    private void closeEntityManager() {
        if (em != null) {
            em.clear();
            em.close();
            em = null;
        }
    }

    public String getDatasourceName() {
        return datasourceName;
    }

    public void setDatasourceName(String datasourceName) {
        this.datasourceName = datasourceName;
    }

    public Date getEndTime() {
        return endTime;
    }

    public void setEndTime(Date endTime) {
        this.endTime = endTime;
    }

    public Date getStartTime() {
        return startTime;
    }

    public void setStartTime(Date startTime) {
        this.startTime = startTime;
    }

}
