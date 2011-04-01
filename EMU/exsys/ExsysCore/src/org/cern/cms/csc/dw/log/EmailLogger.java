package org.cern.cms.csc.dw.log;

import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import jsf.bean.gui.log.BaseLogger;
import jsf.bean.gui.log.Level;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.util.EmailSender;
import org.cern.cms.csc.dw.util.ExceptionPrinter;

/**
 * Logger, which sends emails
 * @author evka
 */
public class EmailLogger extends BaseLogger {

    private static final Logger simpleLogger = SimpleLogger.getLogger(EmailLogger.class);

    private static final Level DEFAULT_LEVEL = Level.CRITICAL; // only used if properties file is not found or ir global (*.email.level) level is not defined in the properties file
    private static final LoggingClassProperties levelProps = new LoggingClassProperties("email.level");
    private static final LoggingClassProperties recipientsProps = new LoggingClassProperties("email.recipients");

    private static final int NUM_SECONDS_TO_VETO = 60 * 10; // number of seconds when all messages of lower or same level will be discarded after one was sent
    private static Level lastMessageLevel = null;
    private static Date lastMessageTime = null;

    private final Class clazz;
    private final int myLevelIntValue;
    private final String recipients;

    private EmailLogger(Class clazz) {
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
        return new EmailLogger(clazz);
    }

    @Override
    public boolean isLevelEnabled(Level level) {
        return (level.intValue() >= myLevelIntValue);
    }

    @Override
    public void log(Level level, String message) {
        if (isLevelEnabled(level)) {
            try {
                // cancel the message if there's too much of them in a short time
                if (lastMessageTime != null) {
                    Calendar cal = new GregorianCalendar();
                    Calendar nowCal = new GregorianCalendar();
                    cal.setTime(lastMessageTime);
                    nowCal.setTime(new Date());
                    cal.add(Calendar.SECOND, NUM_SECONDS_TO_VETO);
                    if (nowCal.after(cal)) {
                        lastMessageTime = null;
                    } else { // subject to cancel
                        if (level.intValue() <= lastMessageLevel.intValue()) { // cancel if severity is same or lower
                            return;
                        }
                    }
                }

                //send email and remember the time and level
                EmailSender.send(recipients, "Expert System " + level, clazz.getName() + ":\n" + message);
                lastMessageTime = new Date();
                lastMessageLevel = level;
            } catch(Exception ex) {
                simpleLogger.error(clazz.getName() + " EmailLogger couldn't send email", ex);
            }
        }
    }

    @Override
    public void log(Level level, Throwable th) {
        if (isLevelEnabled(level)) {
            log(level, ExceptionPrinter.printStackTraceToString(th));
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
            log(level, message + "\n\n" + ExceptionPrinter.printStackTraceToString(th));
        }
    }

    @Override
    public void log(Level level, Object obj) {
        if (isLevelEnabled(level)) {
            log(level, obj.toString());
        }
    }

}
