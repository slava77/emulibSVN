package org.cern.cms.csc.dw.monitor;

import java.util.Date;
import javax.ejb.EJB;
import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.helpers.LogLog;
import org.apache.log4j.spi.ErrorCode;
import org.apache.log4j.spi.LoggingEvent;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.model.monitor.MonitorObject;
import org.cern.cms.csc.dw.util.ServiceLocator;

@EJB(name = "MonitorDaoRef", beanInterface = MonitorDaoLocal.class)
public class MonitorAppender extends AppenderSkeleton {

    private MonitorDaoLocal monitorDao = null;

    private boolean initDatabase() {
        if (this.monitorDao == null) {
            try {
                this.monitorDao = (MonitorDaoLocal) ServiceLocator.getInstance().getEnvService("MonitorDaoRef");
            } catch (Exception ex) {
                errorHandler.error("Exception while getting database", ex, ErrorCode.GENERIC_FAILURE);
            }
        }
        return (monitorDao != null);
    }

    @Override
    public void activateOptions() {
        super.activateOptions();
    }

    @Override
    protected void append(LoggingEvent event) {

        try {
            
            if (event.getMessage() instanceof MonitorObject) {
                
                MonitorObject status = (MonitorObject) event.getMessage();
                status.setTime(new Date(event.timeStamp));
                status.setLevel(event.getLevel().toString());

                if (initDatabase()) {
                    monitorDao.save(status);
                } else {
                    LogLog.error(event.getRenderedMessage());
                }

            } else {

                errorHandler.error("MonitorAppender can log only MonitorObjects", null, ErrorCode.GENERIC_FAILURE);
                
            }
            
        } catch (Exception ex) {
            errorHandler.error("Exception in MonitorAppender.append", ex, ErrorCode.WRITE_FAILURE);
        }

    }

    @Override
    public boolean requiresLayout() {
        return false;
    }

    @Override
    public void close() {
        this.monitorDao = null;
    }
}
