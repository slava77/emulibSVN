package org.cern.cms.csc.dw.dao;

import java.util.Date;
import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.dw.model.monitor.MonitorObject;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;

@Local
public interface MonitorDaoLocal {

    Date getSysdate();
    void save(MonitorObject status);
    List<MonitorQueueStatus> getQueueStatusData(int hours);

}
