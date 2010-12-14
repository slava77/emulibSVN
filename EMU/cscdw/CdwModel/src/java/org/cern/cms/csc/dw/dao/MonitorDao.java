package org.cern.cms.csc.dw.dao;

import java.util.Date;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.model.monitor.MonitorObject;
import org.hibernate.CacheMode;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.hibernate.ejb.EntityManagerImpl;

@Stateless
public class MonitorDao implements MonitorDaoLocal {

    private static Logger logger = Logger.getLogger(MonitorDao.class);

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
    public <T extends MonitorObject> void retentMonitorObjects(Class<T> clazz, Date dateLimit) {
        mem.createQuery("delete from " + clazz.getSimpleName() + " where time < :date")
                .setParameter("date", dateLimit)
                .executeUpdate();
    }

    @SuppressWarnings("unchecked")
    public <T extends MonitorObject> List<T> getMonitorObjects(Class<T> clazz, int lastHours) {
        Session session = getMonitorSession();
        Transaction tr = session.beginTransaction();
        Date from = new Date();
        from.setTime(from.getTime() - lastHours * 60 * 60 * 1000);
        List<T> list = session.createCriteria(clazz)
                .setCacheMode(CacheMode.REFRESH)
                .setCacheable(false)
                .add(Restrictions.gt("time", from))
                .addOrder(Order.asc("time"))
                .list();
        tr.rollback();
        return list;
    }

}
