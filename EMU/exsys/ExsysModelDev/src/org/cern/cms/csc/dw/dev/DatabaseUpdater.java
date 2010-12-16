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
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.metadata.MetadataManager;

public class DatabaseUpdater {

    private final static Logger log = SimpleLogger.getLogger(DatabaseUpdater.class);

    @SuppressWarnings({"static-access","unchecked"})
    public static void main(String[] args) {

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
            for (FactMd f: (new MetadataManager()).getFactMDs()) {
                em.persist(f);
            }

            em.getTransaction().commit();
            em.close();
            emf.close();

        } catch (Exception e) {
            log.error(e);
            e.printStackTrace(System.err);
        }
    }


}
