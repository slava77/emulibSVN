package org.cern.cms.csc.dw.monitor;

import java.util.Date;
import jsf.bean.gui.log.BaseLogger;
import jsf.bean.gui.log.Level;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.log.LoggingClassProperties;
import org.cern.cms.csc.dw.model.monitor.MonitorEntity;
import org.cern.cms.csc.dw.model.monitor.MonitorException;
import org.cern.cms.csc.dw.model.monitor.MonitorLogMessage;
import org.cern.cms.csc.dw.util.EjbLookup;

public class MonitorLogger extends BaseLogger {

    private static final Logger simpleLogger = SimpleLogger.getLogger(MonitorLogger.class);

    private static final Level DEFAULT_LEVEL = Level.INFO; // only used if properties file is not found or ir global (*.monitor.level) level is not defined in the properties file
    private static final LoggingClassProperties levelProps = new LoggingClassProperties("monitor.level");

    private EjbLookup<MonitorDaoLocal> monitorDao = new EjbLookup<MonitorDaoLocal>(EjbLookup.Module.DAO,
                                                                                   MonitorDaoLocal.class);

    private final Class clazz;
    private final int myLevelIntValue;

    private MonitorLogger(Class clazz) {
        this.clazz = clazz;
        String levelStr = levelProps.getBestMatchForClass(clazz);
        if (levelStr == null) {
            myLevelIntValue = DEFAULT_LEVEL.intValue();
        } else {
            myLevelIntValue = Level.valueOf(levelStr).intValue();
        }
    }

    public static Logger getLogger(Class clazz) {
        return new MonitorLogger(clazz);
    }

    public void log(Level level, MonitorEntity obj) {
        try {
            obj.setTime(new Date());
            obj.setLevel(level.name());
            monitorDao.ejbStrict().save(obj);
        } catch (Exception ex) {
            simpleLogger.error("MonitorLogger: Unable to save a MonitorEntity: " + ex.getClass().getCanonicalName() + ": " + ex.getMessage());
        }
    }

    @Override
    public boolean isLevelEnabled(Level level) {
        return (level.intValue() >= myLevelIntValue);
    }

    @Override
    public void log(Level level, Throwable th) {
        if (isLevelEnabled(level)) {
            MonitorException ex = new MonitorException(clazz, th);
            log(level, ex);
        }
    }

    @Override
    public void log(Level level, String message, Object... params) {
        if (isLevelEnabled(level)) {
            log(level, String.format(message, params));
        }
    }

    @Override
    public void log(Level level, String message, Throwable th) {
        if (isLevelEnabled(level)) {
            MonitorException ex = new MonitorException(clazz, message, th);
            log(level, ex);
        }
    }

    @Override
    public void log(Level level, String message) {
        if (isLevelEnabled(level)) {
            MonitorLogMessage logMsg = new MonitorLogMessage(clazz, message);
            log(level, logMsg);
        }
    }

    @Override
    public void log(Level level, Object obj) {
        if (isLevelEnabled(level)) {
            if (obj instanceof MonitorEntity) {
                log(level, (MonitorEntity) obj);
            } else {
                log(level, obj.toString());
            }
        }
    }

}
