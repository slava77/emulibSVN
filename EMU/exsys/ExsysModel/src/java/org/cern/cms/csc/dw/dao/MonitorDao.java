package org.cern.cms.csc.dw.dao;

import java.util.Date;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.dao.table.BeanTableDao;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.model.monitor.MonitorEntity;
import org.hibernate.CacheMode;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.hibernate.ejb.EntityManagerImpl;

@Stateless
public class MonitorDao extends BeanTableDao implements MonitorDaoLocal {

    private static Logger logger = SimpleLogger.getLogger(MonitorDao.class);

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    @PersistenceContext(unitName="CdwMonitorPU")
    private EntityManager mem;

    
    @Override
    public Date getSysdate() {
        return (Date) em.createNativeQuery("select sysdate from dual").getSingleResult();
    }

    @Override
    public void save(MonitorEntity status) {
        mem.persist(status);
        mem.flush();
    }

    @SuppressWarnings("unchecked")
    @Override
    public <T extends MonitorEntity> void retentMonitorObjects(Class<T> clazz, Date dateLimit) {
        mem.createQuery("delete from " + clazz.getSimpleName() + " where time < :date")
                .setParameter("date", dateLimit)
                .executeUpdate();
    }

    @SuppressWarnings("unchecked")
    @Override
    public <T extends MonitorEntity> List<T> getMonitorObjects(Class<T> clazz, int lastHours) {
        Session session = getSession();
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

    @Override
    public Session getSession() {
        return (Session) ((EntityManagerImpl) mem.getDelegate()).getDelegate();
    }

}
