package org.cern.cms.csc.dw.dev;

import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;

public class OntologyGraphToDatabaseImporter {

    private static final Logger log = SimpleLogger.getLogger(OntologyGraphToDatabaseImporter.class);

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

    public OntologyGraphToDatabaseImporter(EntityManager em, String graphdb) {
        this.em = em;
        gfactory = new GOntologyFactory(graphdb);
        factory = new OntologyFactory();
    }


    @SuppressWarnings("static-access")
    public static void main(String[] args) {

        try {

            // create the command line parser
            CommandLineParser clparser = new PosixParser();
            Options opts = new Options();
            opts.addOption(OptionBuilder.hasArg().withDescription("Graph DB folder").create("gdb"));
            
            CommandLine cmdLine = clparser.parse(opts, args);

            EntityManagerFactory emf = Persistence.createEntityManagerFactory("CdwUtilPU");
            EntityManager em = emf.createEntityManager();

            OntologyGraphToDatabaseImporter importer = new OntologyGraphToDatabaseImporter(em, cmdLine.getOptionValue("gdb"));
            importer.process();

            em.close();
            emf.close();

        } catch (Exception e) {
            log.error(e);
            e.printStackTrace(System.err);
        }
    }

}
