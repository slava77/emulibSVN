package org.cern.cms.csc.dw.dao;

import javax.ejb.EJB;
import javax.ejb.Stateless;

@Stateless
public class EditorDao implements EditorDaoLocal {

    @EJB
    private EntityDaoLocal entityDao;

    @EJB
    private OntologyDaoLocal ontologyDao;

    @EJB
    private GOntologyDaoLocal gOntologyDao;

    @Override
    public EntityDaoLocal getEntityDao() {
        return entityDao;
    }

    @Override
    public OntologyDaoLocal getOntologyDao() {
        return ontologyDao;
    }

    @Override
    public GOntologyDaoLocal getGOntologyDao() {
        return gOntologyDao;
    }

    

}
