package org.cern.cms.csc.dw.log;

import jsf.bean.gui.log.BaseLogger;
import jsf.bean.gui.log.Level;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.util.ExceptionPrinter;
import org.cern.cms.csc.dw.util.SmsSender;

/**
 * Logger, which sends SMS'es
 * @author evka
 */
public class KillerLogger extends BaseLogger {

    private static final Logger simpleLogger = SimpleLogger.getLogger(KillerLogger.class);

    private static final Level DEFAULT_LEVEL = Level.FATAL; // only used if properties file is not found or ir global (*.kill.level) level is not defined in the properties file
    private static final LoggingClassProperties levelProps = new LoggingClassProperties("kill.level");

    private final Class clazz;
    private final int myLevelIntValue;

    private KillerLogger(Class clazz) {
        this.clazz = clazz;
        String levelStr = levelProps.getBestMatchForClass(clazz);
        myLevelIntValue = (levelStr != null ? Level.valueOf(levelStr).intValue() : DEFAULT_LEVEL.intValue());
    }

    public static Logger getLogger(Class clazz) {
        return new KillerLogger(clazz);
    }

    @Override
    public boolean isLevelEnabled(Level level) {
        return (level.intValue() >= myLevelIntValue);
    }

    private void doKill() {
        System.exit(-1);
    }

    @Override
    public void log(Level level, String message) {
        if (isLevelEnabled(level)) {
            simpleLogger.fatal(clazz.getSimpleName() + " KillerLogger: Killing glassfish because of this message: " + message);
            doKill();
        }
    }

    @Override
    public void log(Level level, Throwable th) {
        if (isLevelEnabled(level)) {
            simpleLogger.fatal(clazz.getSimpleName() + " KillerLogger: Killing glassfish because of this exception: \n"
                                           + ExceptionPrinter.printStackTraceToString(th));
            doKill();
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
            simpleLogger.fatal(clazz.getSimpleName() + " KillerLogger: Killing glassfish because of this message: \""
                                           + message + "\" and exception: \n"
                                           + ExceptionPrinter.printStackTraceToString(th));
            doKill();
        }
    }

    @Override
    public void log(Level level, Object obj) {
        if (isLevelEnabled(level)) {
            log(level, obj.toString());
        }
    }

}
