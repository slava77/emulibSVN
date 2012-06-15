package jsf.bean.gui.log;

public class SimpleLogger extends BaseLogger {

    private final java.util.logging.Logger logger;
    private final Class clazz;
    private final StackTraceElement defaultStackTraceElement;

    private SimpleLogger(Class clazz) {
        this.logger = java.util.logging.Logger.getLogger(clazz.getName());
        this.clazz = clazz;
        this.defaultStackTraceElement = new StackTraceElement(clazz.getCanonicalName(), "", "", 0);
    }

    public static Logger getLogger(Class clazz) {
        return new SimpleLogger(clazz);
    }

    private StackTraceElement getStackTraceElement() {
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
        for (StackTraceElement el: stackTrace) {
            if (el.getClassName().startsWith(clazz.getCanonicalName())) {
                return el;
            }
        }
        return defaultStackTraceElement;
    }

    @Override
    public boolean isLevelEnabled(Level level) {
        return logger.isLoggable(level.value());
    }

    /**
     * Generic log
     */

    @Override
    public void log(Level level, String message, Object... params) {
        StackTraceElement el = getStackTraceElement();
        logger.logp(level.value(), el.getClassName(), el.getMethodName(), message, params);
    }

    @Override
    public void log(Level level, String message, Throwable th) {
        StackTraceElement el = getStackTraceElement();
        logger.logp(level.value(), el.getClassName(), el.getMethodName(), message, th);
    }

    @Override
    public void log(Level level, String message) {
        StackTraceElement el = getStackTraceElement();
        logger.logp(level.value(), el.getClassName(), el.getMethodName(), message);
    }

    @Override
    public void log(Level level, Throwable th) {
        StackTraceElement el = getStackTraceElement();
        logger.logp(level.value(), el.getClassName(), el.getMethodName(), null, th);
    }

    @Override
    public void log(Level level, Object obj) {
        StackTraceElement el = getStackTraceElement();
        logger.logp(level.value(), el.getClassName(), el.getMethodName(), obj.toString());
    }

}
