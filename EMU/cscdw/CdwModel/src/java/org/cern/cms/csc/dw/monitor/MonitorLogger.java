package org.cern.cms.csc.dw.monitor;

import org.apache.log4j.Logger;

public class MonitorLogger {

    private static final String MONITOR_LOGGER_NAME = "MONITOR";

    public static Logger getLogger() {
        return Logger.getLogger(MONITOR_LOGGER_NAME);
    }

}
