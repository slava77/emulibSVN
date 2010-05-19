/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.cern.cms.csc.dw.dev;

import java.util.List;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.metadata.MetadataManager;

public class DatabaseUpdater {

    static Logger log = Logger.getLogger(DatabaseUpdater.class.getName());

    @SuppressWarnings({"static-access","unchecked"})
    public static void main(String[] args) {

        MetadataManager mm = new MetadataManager();

        try {

            // create the command line parser
            CommandLineParser clparser = new PosixParser();

            Options opts = new Options();
            CommandLine cmdLine = clparser.parse(opts, args);

            EntityManagerFactory emf = Persistence.createEntityManagerFactory("CdwUtilPU");
            EntityManager em = emf.createEntityManager();
            em.getTransaction().begin();

            // Remove existing fact metadata
            List<FactMd> currFactMd = em.createQuery("select m from org.cern.cms.csc.dw.metadata.FactMd m").getResultList();
            for (FactMd f: currFactMd) {
                em.remove(f);
            }

            em.flush();

            // Write current fact metadata
            for (FactMd f: mm.getFactClasses()) {
                em.persist(f);
            }

            em.getTransaction().commit();
            em.close();
            emf.close();

        } catch (Exception e) {
            log.fatal(e);
            e.printStackTrace(System.err);
        }
    }


}
