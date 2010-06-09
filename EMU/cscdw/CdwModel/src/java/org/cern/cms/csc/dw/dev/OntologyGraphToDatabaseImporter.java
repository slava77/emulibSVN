package org.cern.cms.csc.dw.dev;

import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.GOntologyFactory;
import org.cern.cms.csc.dw.dao.OntologyFactory;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;

public class OntologyGraphToDatabaseImporter {

    static Logger log = Logger.getLogger(OntologyGraphToDatabaseImporter.class.getName());

    private EntityManager em = null;
    private GOntologyFactory gfactory;
    private OntologyFactory factory;
    
    public void process() throws Exception {

        try {
            
            em.getTransaction().begin();

            log.info("Persisting component classes..");
            {

                for (GComponentClass gcc : gfactory.getGComponentClasses()) {
                    ComponentClass cc = factory.getComponentClass(gcc);
                    em.persist(cc);
                }
            }

            log.info("Persisting component link classes..");
            {
                for (GComponentLinkClass gclc : gfactory.getGComponentLinkClasses()) {
                    ComponentLinkClass clc = factory.getComponentLinkClass(gclc);
                    em.persist(clc);
                }
            }

            log.info("Persisting components..");
            {
                for (GComponent gc : gfactory.getGComponents()) {
                    Component c = factory.getComponent(gc);
                    em.persist(c);
                }
            }

            log.info("Commit..");
            em.getTransaction().commit();
            em.getTransaction().begin();

        }

        finally {
            em.getTransaction().rollback();
            gfactory.close();
        }

    }

    public OntologyGraphToDatabaseImporter(EntityManager em) {
        this.em = em;
        gfactory = new GOntologyFactory();
        factory = new OntologyFactory();
    }


    @SuppressWarnings("static-access")
    public static void main(String[] args) {

        try {

            // create the command line parser
            //CommandLineParser clparser = new PosixParser();
            //Options opts = new Options();
            //opts.addOption(OptionBuilder.hasArg().withDescription("OWL file").create("owl"));
            //CommandLine cmdLine = clparser.parse(opts, args);

            EntityManagerFactory emf = Persistence.createEntityManagerFactory("CdwUtilPU");
            EntityManager em = emf.createEntityManager();

            OntologyGraphToDatabaseImporter importer = new OntologyGraphToDatabaseImporter(em);
            importer.process();

            em.close();
            emf.close();

        } catch (Exception e) {
            log.fatal(e);
            e.printStackTrace(System.err);
        }
    }

}
