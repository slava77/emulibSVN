package jsf.bean.gui.log;

public enum Level {

    OFF(java.util.logging.Level.OFF, Integer.MAX_VALUE),
    FATAL(java.util.logging.Level.SEVERE, 1200),
    CRITICAL(java.util.logging.Level.SEVERE, 1100),
    ERROR(java.util.logging.Level.SEVERE, 1000),
    WARN(java.util.logging.Level.WARNING, 900),
    INFO(java.util.logging.Level.INFO, 800),
    CONFIG(java.util.logging.Level.CONFIG, 700),
    DEBUG(java.util.logging.Level.FINE, 500),
    TRACE(java.util.logging.Level.FINEST, 300);

    private final java.util.logging.Level level;
    private final int intValue;

    Level(java.util.logging.Level level, int intValue) {
        this.level = level;
        this.intValue = intValue;
    }

    public java.util.logging.Level value() {
        return level;
    }

    public int intValue() {
        return intValue;
    }

    public static Level fromValue(java.util.logging.Level level) {
        for (Level l : Level.values()) {
            if (l.value().equals(level)) {
                return l;
            }
        }
        throw new IllegalArgumentException(level.getName());
    }
    
}
