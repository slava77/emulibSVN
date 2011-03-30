package org.cern.cms.csc.dw.log;

import jsf.bean.gui.log.BaseLogger;
import jsf.bean.gui.log.Level;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.util.SmsSender;

/**
 * Logger, which sends SMS'es
 * @author evka
 */
public class SmsLogger extends BaseLogger {

    private static final Logger simpleLogger = SimpleLogger.getLogger(SmsLogger.class);

    private static final Level DEFAULT_LEVEL = Level.CRITICAL; // only used if properties file is not found or ir global (*.sms.level) level is not defined in the properties file
    private static final LoggingClassProperties levelProps = new LoggingClassProperties("sms.level");
    private static final LoggingClassProperties recipientsProps = new LoggingClassProperties("sms.recipients");

    private final Class clazz;
    private final int myLevelIntValue;
    private final String recipients;

    private SmsLogger(Class clazz) {
        this.clazz = clazz;
        String rec = recipientsProps.getBestMatchForClass(clazz);
        recipients = (rec != null ? rec : "");
        if (recipients.isEmpty()) {
            myLevelIntValue = Level.OFF.intValue();
        } else {
            String levelStr = levelProps.getBestMatchForClass(clazz);
            myLevelIntValue = (levelStr != null ? Level.valueOf(levelStr).intValue() : DEFAULT_LEVEL.intValue());
        }
    }

    public static Logger getLogger(Class clazz) {
        return new SmsLogger(clazz);
    }

    @Override
    public boolean isLevelEnabled(Level level) {
        return (level.intValue() >= myLevelIntValue);
    }

    @Override
    public void log(Level level, String message) {
        if (isLevelEnabled(level)) {
            try {
                SmsSender.send(recipients, level.toString(), message);
            } catch(Exception ex) {
                simpleLogger.error("SmsLogger couldn't send email", ex);
            }
        }
    }

    @Override
    public void log(Level level, Throwable th) {
        if (isLevelEnabled(level)) {
            log(level, th.getClass().getSimpleName() + ": " + th.getMessage());
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
            log(level, message + "\n" + th.getClass().getSimpleName() + ": " + th.getMessage());
        }
    }

    @Override
    public void log(Level level, Object obj) {
        if (isLevelEnabled(level)) {
            log(level, obj.toString());
        }
    }

}
