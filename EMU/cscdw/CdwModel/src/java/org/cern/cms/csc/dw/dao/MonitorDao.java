package org.cern.cms.csc.dw.dao;

import java.util.Date;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.model.monitor.MonitorObject;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.hibernate.ejb.EntityManagerImpl;

@Stateless
public class MonitorDao implements MonitorDaoLocal {

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    @PersistenceContext(unitName="CdwMonitorPU")
    private EntityManager mem;

    private Session getMonitorSession() {
        return (Session)((EntityManagerImpl) mem.getDelegate()).getDelegate();
    }
    
    public Date getSysdate() {
        return (Date) em.createNativeQuery("select sysdate from dual").getSingleResult();
    }

    public void save(MonitorObject status) {
        mem.persist(status);
        mem.flush();
    }

    @SuppressWarnings("unchecked")
    public List<MonitorQueueStatus> getQueueStatusData(int hours) {
        Session session = getMonitorSession();
        Transaction tr = session.beginTransaction();
        Date from = new Date();
        from.setTime(from.getTime() - hours * 60 * 60 * 1000);
        List<MonitorQueueStatus> list = session.createCriteria(MonitorQueueStatus.class)
                .add(Restrictions.gt("time", from))
                .addOrder(Order.asc("time"))
                .list();
        tr.rollback();
        return list;
    }

}
