package org.cern.cms.csc.dw.log;

public interface Logger {

    boolean isConfigEnabled();

    boolean isDebugEnabled();

    boolean isErrorEnabled();

    boolean isInfoEnabled();

    boolean isTraceEnabled();

    boolean isWarnEnabled();

    /**
     * Config level
     */
    void config(String message, Object... params);

    void config(String message, Throwable th);

    void config(String message);

    void config(Throwable th);

    void config(Object obj);

    /**
     * Debug level
     */
    void debug(String message, Object... params);

    void debug(String message, Throwable th);

    void debug(String message);

    void debug(Throwable th);

    void debug(Object obj);

    /**
     * Error level
     */
    void error(String message, Object... params);

    void error(String message, Throwable th);

    void error(String message);

    void error(Throwable th);

    void error(Object obj);

    /**
     * Info level
     */
    void info(String message, Object... params);

    void info(String message, Throwable th);

    void info(String message);

    void info(Throwable th);

    void info(Object obj);

    /**
     * Generic log
     */
    void log(Level level, String message, Object... params);

    void log(Level level, String message, Throwable th);

    void log(Level level, String message);

    void log(Level level, Throwable th);

    void log(Level level, Object obj);

    /**
     * Trace level
     */
    void trace(String message, Object... params);

    void trace(String message, Throwable th);

    void trace(String message);

    void trace(Throwable th);

    void trace(Object obj);

    /**
     * Error level
     */
    void warn(String message, Object... params);

    void warn(String message, Throwable th);

    void warn(String message);

    void warn(Throwable th);

    void warn(Object obj);

}
