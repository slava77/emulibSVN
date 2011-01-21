package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;

@Local
public interface GenericDaoLocal {

    EntityDaoLocal getEntityDao();

    OntologyDaoLocal getOntologyDao();

    GOntologyDaoLocal getGOntologyDao();

}
