package org.cern.cms.csc.exsys.knowledgebase;

import java.io.IOException;
import java.net.URI;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Logger;
import org.coode.manchesterowlsyntax.ManchesterOWLSyntaxEditorParser;
import org.semanticweb.owl.apibinding.OWLManager;
import org.semanticweb.owl.expression.OWLEntityChecker;
import org.semanticweb.owl.expression.ParserException;
import org.semanticweb.owl.expression.ShortFormEntityChecker;
import org.semanticweb.owl.inference.OWLReasoner;
import org.semanticweb.owl.inference.OWLReasonerException;
import org.semanticweb.owl.inference.OWLReasonerFactory;
import org.semanticweb.owl.model.*;
import org.semanticweb.owl.util.CachingBidirectionalShortFormProvider;
import org.semanticweb.owl.util.DLExpressivityChecker;
import org.semanticweb.owl.util.ReferencedEntitySetProvider;
import org.semanticweb.owl.util.ShortFormProvider;
import org.semanticweb.owl.util.SimpleShortFormProvider;
import org.semanticweb.reasonerfactory.factpp.FaCTPlusPlusReasonerFactory;

/**
 *
 * @author Evaldas Juska
 */
public class KnowledgeBase {

    private static final Logger log = Logger.getLogger(KnowledgeBase.class.getName());

    private String ontologyURIStr;
    private OWLOntologyManager manager;
    private OWLOntology ontology;
    private OWLReasoner reasoner;
    private OWLEntityChecker owlEntityChecker;
    private ShortFormProvider shortFormProvider;
    private CachingBidirectionalShortFormProvider nameCache;

    public KnowledgeBase() {
    }

    /** Constructor.
     *
     * @param OntologyURI - ontology URI (location of the owl file)
     * @throws org.semanticweb.owl.model.OWLOntologyCreationException
     * @throws org.semanticweb.owl.inference.OWLReasonerException
     */
    public KnowledgeBase(String ontologyURI) throws OWLOntologyCreationException, OWLReasonerException {
        setOntologyURIStr(ontologyURI);
        init();
    }

    public void init() throws OWLOntologyCreationException, OWLReasonerException {
        if (getOntologyURIStr() == null) {
            throw new OWLOntologyCreationException("Ontology URI is null");
        }
        
        // load the ontology
        URI ontologyURI = URI.create(getOntologyURIStr());
        manager = OWLManager.createOWLOntologyManager();
        ontology = manager.loadOntologyFromPhysicalURI(ontologyURI);
        log.info("Loaded ontology from: " + ontologyURI);

        // create a reasoner
        log.info("Instantiating reasoner");
        OWLReasonerFactory factory = new FaCTPlusPlusReasonerFactory();
        reasoner = factory.createReasoner(manager);
        Set<OWLOntology> importsClosure = manager.getImportsClosure(ontology);
        reasoner.loadOntologies(importsClosure);
        reasoner.classify();
        log.info("Reasoner instantiated and ontology classified");
        //------------------

        // check the ontology
        DLExpressivityChecker checker = new DLExpressivityChecker(importsClosure);
        log.info("Expressivity: " + checker.getDescriptionLogicName());
        boolean consistent = reasoner.isConsistent(ontology);
        log.info("Ontrology consistent: " + consistent);
        Set<OWLClass> inconsistentClasses = reasoner.getInconsistentClasses();
        if (!inconsistentClasses.isEmpty()) {
            for (OWLClass inconsistentClass : inconsistentClasses) {
                log.info("Inconsistent class: " + inconsistentClass);
            }
        } else {
            log.info("There are no inconsistent classes.");
        }
    }

    /** Executes a given DL query in a reasoner and returns individuals. */
    public Set<OWLIndividual> getIndividuals(String query) throws IOException, ParserException, OWLReasonerException {
        ManchesterOWLSyntaxEditorParser parser = getExpressionParser(query);
        OWLDescription desc = parser.parseDescription();

        return getReasoner().getIndividuals(desc, true);
    }

    public ManchesterOWLSyntaxEditorParser getExpressionParser(String expression) {
        ManchesterOWLSyntaxEditorParser parser = new ManchesterOWLSyntaxEditorParser(getManager().getOWLDataFactory(), expression);
        parser.setOWLEntityChecker(getEntityChecker());
        return parser;
    }

    public OWLEntityChecker getEntityChecker() {
        if (owlEntityChecker == null){
            owlEntityChecker = new ShortFormEntityChecker(getNameCache());
        }
        return owlEntityChecker;
    }

    private CachingBidirectionalShortFormProvider getNameCache() {
        if (nameCache == null) {
            nameCache = new CachingBidirectionalShortFormProvider() {

                @Override
                protected String generateShortForm(OWLEntity entity) {
                    return getShortFormProvider().getShortForm(entity);
                }
            };
            Set<OWLOntology> onts = new HashSet<OWLOntology>();
            onts.add(getOntology());
            nameCache.rebuild(new ReferencedEntitySetProvider(onts));
        }
        return nameCache;
    }

    private ShortFormProvider getShortFormProvider() {
        if (shortFormProvider == null) {
            shortFormProvider = new SimpleShortFormProvider();
        }
        return shortFormProvider;
    }


    /**
     * @return the manager
     */
    public OWLOntologyManager getManager() {
        return manager;
    }

    /**
     * @return the ontology
     */
    public OWLOntology getOntology() {
        return ontology;
    }

    /**
     * @return the reasoner
     */
    public OWLReasoner getReasoner() {
        return reasoner;
    }

    /**
     * @return Ontology physical URI as string
     */
    public String getOntologyURIStr() {
        return ontologyURIStr;
    }

    /**
     * @param ontologyURIStr Ontology physical URI as string
     */
    public void setOntologyURIStr(String OntologyURIStr) {
        this.ontologyURIStr = OntologyURIStr;
    }
}
