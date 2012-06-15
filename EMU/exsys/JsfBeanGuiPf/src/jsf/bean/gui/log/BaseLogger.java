package jsf.bean.gui.log;

public abstract class BaseLogger implements Logger {

    /**
     * Fatal level
     */

    @Override
    public void fatal(String message, Object... params) {
        log(Level.FATAL, message, params);
    }

    @Override
    public void fatal(String message, Throwable th) {
        log(Level.FATAL, message, th);
    }

    @Override
    public void fatal(String message) {
        log(Level.FATAL, message);
    }

    @Override
    public void fatal(Throwable th) {
        log(Level.FATAL, th);
    }

    @Override
    public void fatal(Object obj) {
        log(Level.FATAL, obj);
    }

    /**
     * Critical level
     */

    @Override
    public void critical(String message, Object... params) {
        log(Level.CRITICAL, message, params);
    }

    @Override
    public void critical(String message, Throwable th) {
        log(Level.CRITICAL, message, th);
    }

    @Override
    public void critical(String message) {
        log(Level.CRITICAL, message);
    }

    @Override
    public void critical(Throwable th) {
        log(Level.CRITICAL, th);
    }

    @Override
    public void critical(Object obj) {
        log(Level.CRITICAL, obj);
    }

    /**
     * Error level
     */

    @Override
    public void error(String message, Object... params) {
        log(Level.ERROR, message, params);
    }

    @Override
    public void error(String message, Throwable th) {
        log(Level.ERROR, message, th);
    }

    @Override
    public void error(String message) {
        log(Level.ERROR, message);
    }

    @Override
    public void error(Throwable th) {
        log(Level.ERROR, th);
    }

    @Override
    public void error(Object obj) {
        log(Level.ERROR, obj);
    }

    /**
     * Warning level
     */

    @Override
    public void warn(String message, Object... params) {
        log(Level.WARN, message, params);
    }

    @Override
    public void warn(String message, Throwable th) {
        log(Level.WARN, message, th);
    }

    @Override
    public void warn(String message) {
        log(Level.WARN, message);
    }

    @Override
    public void warn(Throwable th) {
        log(Level.WARN, th);
    }

    @Override
    public void warn(Object obj) {
        log(Level.WARN, obj);
    }

    /**
     * Config level
     */

    @Override
    public void config(String message, Object... params) {
        log(Level.CONFIG, message, params);
    }

    @Override
    public void config(String message, Throwable th) {
        log(Level.CONFIG, message, th);
    }

    @Override
    public void config(String message) {
        log(Level.CONFIG, message);
    }

    @Override
    public void config(Throwable th) {
        log(Level.CONFIG, th);
    }

    @Override
    public void config(Object obj) {
        log(Level.CONFIG, obj);
    }

    /**
     * Info level
     */

    @Override
    public void info(String message, Object... params) {
        log(Level.INFO, message, params);
    }

    @Override
    public void info(String message, Throwable th) {
        log(Level.INFO, message, th);
    }

    @Override
    public void info(String message) {
        log(Level.INFO, message);
    }

    @Override
    public void info(Throwable th) {
        log(Level.INFO, th);
    }

    @Override
    public void info(Object obj) {
        log(Level.INFO, obj);
    }

    /**
     * Debug level
     */

    @Override
    public void debug(String message, Object... params) {
        log(Level.DEBUG, message, params);
    }

    @Override
    public void debug(String message, Throwable th) {
        log(Level.DEBUG, message, th);
    }

    @Override
    public void debug(String message) {
        log(Level.DEBUG, message);
    }

    @Override
    public void debug(Throwable th) {
        log(Level.DEBUG, th);
    }

    @Override
    public void debug(Object obj) {
        log(Level.DEBUG, obj);
    }

    /**
     * Trace level
     */

    @Override
    public void trace(String message, Object... params) {
        log(Level.TRACE, message, params);
    }

    @Override
    public void trace(String message, Throwable th) {
        log(Level.TRACE, message, th);
    }

    @Override
    public void trace(String message) {
        log(Level.TRACE, message);
    }

    @Override
    public void trace(Throwable th) {
        log(Level.TRACE, th);
    }
    
    @Override
    public void trace(Object obj) {
        log(Level.TRACE, obj);
    }

    /**
     * Is enabled implementations
     */

    public boolean isFatalEnabled() {
        return isLevelEnabled(Level.FATAL);
    }

    public boolean isCriticalEnabled() {
        return isLevelEnabled(Level.CRITICAL);
    }

    @Override
    public boolean isErrorEnabled() {
        return isLevelEnabled(Level.ERROR);
    }

    @Override
    public boolean isWarnEnabled() {
        return isLevelEnabled(Level.WARN);
    }

    @Override
    public boolean isConfigEnabled() {
        return isLevelEnabled(Level.CONFIG);
    }

    @Override
    public boolean isInfoEnabled() {
        return isLevelEnabled(Level.INFO);
    }

    @Override
    public boolean isDebugEnabled() {
        return isLevelEnabled(Level.DEBUG);
    }

    @Override
    public boolean isTraceEnabled() {
        return isLevelEnabled(Level.TRACE);
    }

}
