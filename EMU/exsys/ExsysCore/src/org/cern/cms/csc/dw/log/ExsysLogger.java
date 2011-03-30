/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.log;

import java.util.ArrayList;
import java.util.List;
import jsf.bean.gui.log.BaseLogger;
import jsf.bean.gui.log.Level;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.monitor.MonitorLogger;

/**
 *
 * @author evka
 */
public class ExsysLogger extends BaseLogger {

    private List<Logger> delegateLoggers;

    private ExsysLogger(Class clazz) {
        delegateLoggers = new ArrayList<Logger>();
        delegateLoggers.add(SimpleLogger.getLogger(clazz));
        delegateLoggers.add(MonitorLogger.getLogger(clazz));
        delegateLoggers.add(EmailLogger.getLogger(clazz));
        delegateLoggers.add(SmsLogger.getLogger(clazz));
        delegateLoggers.add(KillerLogger.getLogger(clazz));
    }

    public static ExsysLogger getLogger(Class clazz) {
        return new ExsysLogger(clazz);
    }

    public boolean isLevelEnabled(Level level) {
        return true;
    }

    public void log(Level level, String message, Object... params) {
        for (Logger logger: delegateLoggers) {
            logger.log(level, message, params);
        }
    }

    public void log(Level level, String message, Throwable th) {
        for (Logger logger: delegateLoggers) {
            logger.log(level, message, th);
        }
    }

    public void log(Level level, String message) {
        for (Logger logger: delegateLoggers) {
            logger.log(level, message);
        }
    }

    public void log(Level level, Throwable th) {
        for (Logger logger: delegateLoggers) {
            logger.log(level, th);
        }
    }

    public void log(Level level, Object obj) {
        for (Logger logger: delegateLoggers) {
            logger.log(level, obj);
        }
    }

}
