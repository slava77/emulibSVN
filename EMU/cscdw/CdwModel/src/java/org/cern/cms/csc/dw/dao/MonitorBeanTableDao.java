package org.cern.cms.csc.dw.dao;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.dao.table.BeanTableDao;
import org.hibernate.Session;
import org.hibernate.ejb.EntityManagerImpl;

@Stateless
public class MonitorBeanTableDao extends BeanTableDao implements MonitorBeanTableDaoLocal {

    @PersistenceContext(unitName="CdwMonitorPU")
    private EntityManager em;

    protected Session getSession() {
        return (Session)((EntityManagerImpl) em.getDelegate()).getDelegate();
    }

}
