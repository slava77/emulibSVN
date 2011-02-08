package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;

@Local
public interface EditorDaoLocal {

    public EntityDaoLocal getEntityDao();

    public OntologyDaoLocal getOntologyDao();

    public GOntologyDaoLocal getGOntologyDao();

}
