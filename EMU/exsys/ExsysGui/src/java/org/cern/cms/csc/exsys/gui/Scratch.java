/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import jsf.bean.gui.log.Level;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.dw.util.ExsysIORemoteEjbLookup;
import org.cern.cms.csc.exsys.io.DimServiceProviderRemote;
import org.cern.cms.csc.exsys.re.model.DcsCommandAction;
import org.cern.cms.csc.exsys.re.model.DimPublicationAction;

/**
 *
 * @author evka
 */
@ManagedBean
@RequestScoped
public class Scratch {

    private static final Logger logger = ExsysLogger.getLogger(Scratch.class);

    private EjbLookup<DimServiceProviderRemote> dimServiceProvider =
                      new ExsysIORemoteEjbLookup<DimServiceProviderRemote>(DimServiceProviderRemote.class);

    private Level loggerTestLevel;
    private String dcsTestCommandArgs = "";

    /** Creates a new instance of Scratch */
    public Scratch() {
    }

    public void testLogger() {
        logger.log(loggerTestLevel, "Something " + loggerTestLevel + " just happened");
    }

    public Level getLoggerTestLevel() {
        return loggerTestLevel;
    }

    public void setLoggerTestLevel(Level loggerTestLevel) {
        this.loggerTestLevel = loggerTestLevel;
    }

    public Level[] getAllLoggerLevels() {
        return Level.values();
    }

    public String getDcsTestCommandArgs() {
        return dcsTestCommandArgs;
    }

    public void setDcsTestCommandArgs(String dcsTestCommandArgs) {
        this.dcsTestCommandArgs = dcsTestCommandArgs;
    }

    public void sendTestDcsCommand() throws Exception {
        String dimData = "test|" + dcsTestCommandArgs.replaceAll(",", "|");
        DimPublicationAction dcsCommandAction = new DcsCommandAction();
        dimServiceProvider.ejbStrict().publishString(dcsCommandAction, dimData);
        Thread.sleep(1000);
        dimServiceProvider.ejbStrict().publishString(dcsCommandAction, "");

    }
}
