package org.cern.cms.csc.dw.log;

public enum Level {

    ERROR(java.util.logging.Level.SEVERE), 
    WARN(java.util.logging.Level.WARNING),
    CONFIG(java.util.logging.Level.CONFIG),
    INFO(java.util.logging.Level.INFO),
    DEBUG(java.util.logging.Level.FINE),
    TRACE(java.util.logging.Level.FINEST);

    private final java.util.logging.Level level;

    Level(java.util.logging.Level level) {
        this.level = level;
    }

    public java.util.logging.Level value() {
        return level;
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
