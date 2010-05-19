package org.cern.cms.csc.dw;

import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;

public abstract class TestBase {

    private EntityManagerFactory emf;
    protected EntityManager em;

    public TestBase() {
        this("CdwUtilPU");
    }

    public TestBase(String persistenceUnitName) {
        emf = Persistence.createEntityManagerFactory(persistenceUnitName);
        em = emf.createEntityManager();
    }

    @Override
    protected void finalize() throws Throwable {
        em.close();
        emf.close();
    }



}
