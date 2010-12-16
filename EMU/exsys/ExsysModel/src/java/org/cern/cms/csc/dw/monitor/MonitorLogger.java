package org.cern.cms.csc.dw.monitor;

import java.util.Date;
import org.cern.cms.csc.dw.dao.MonitorDao;
import org.cern.cms.csc.dw.log.Level;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.log.BaseLogger;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.model.monitor.MonitorEntity;
import org.cern.cms.csc.dw.model.monitor.MonitorException;
import org.cern.cms.csc.dw.util.EjbLookup;

public class MonitorLogger extends BaseLogger {

    private static final Logger logger = SimpleLogger.getLogger(MonitorLogger.class);

    private EjbLookup<MonitorDaoLocal> monitorDao = new EjbLookup<MonitorDaoLocal>(MonitorDaoLocal.class, MonitorDao.class);

    private final Class clazz;

    private MonitorLogger(Class clazz) {
        this.clazz = clazz;
    }

    public static Logger getLogger(Class clazz) {
        return new MonitorLogger(clazz);
    }

    public void log(Level level, MonitorEntity obj) {
        obj.setTime(new Date());
        obj.setLevel(level.name());
        monitorDao.ejb().save(obj);
    }

    @Override
    public boolean isConfigEnabled() {
        return true;
    }

    @Override
    public boolean isDebugEnabled() {
        return true;
    }

    @Override
    public boolean isErrorEnabled() {
        return true;
    }

    @Override
    public boolean isInfoEnabled() {
        return true;
    }

    @Override
    public boolean isTraceEnabled() {
        return true;
    }

    @Override
    public boolean isWarnEnabled() {
        return true;
    }

    @Override
    public void log(Level level, Throwable th) {
        MonitorException ex = new MonitorException(clazz, th);
        log(level, ex);
    }

    @Override
    public void log(Level level, String message, Object... params) {
        log(level, String.format(message, params));
    }

    @Override
    public void log(Level level, String message, Throwable th) {
        MonitorException ex = new MonitorException(clazz, message, th);
        log(level, ex);
    }

    @Override
    public void log(Level level, String message) {
        MonitorException ex = new MonitorException(clazz, message);
        log(level, ex);
    }

    @Override
    public void log(Level level, Object obj) {
        if (obj instanceof MonitorEntity) {
            log(level, (MonitorEntity) obj);
        } else {
            log(level, obj.toString());
        }
    }

}
