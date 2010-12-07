package org.cern.cms.csc.dw.dao;

import java.util.Date;
import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.model.monitor.MonitorObject;

@Local
public interface MonitorDaoLocal {

    Date getSysdate();
    void save(MonitorObject status);
    
    public <T extends MonitorObject> List<T> getMonitorObjects(
            Class<T> clazz,
            int lastHours);

}
