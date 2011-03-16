package org.cern.cms.csc.dw.dao;

import java.util.Date;
import java.util.List;
import javax.ejb.Local;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.model.monitor.MonitorEntity;

@Local
public interface MonitorDaoLocal extends BeanTableDaoIf {

    Date getSysdate();
    
    void save(MonitorEntity status);
    
    public <T extends MonitorEntity> List<T> getMonitorObjects(
            Class<T> clazz,
            int lastHours);

    public <T extends MonitorEntity> void retentMonitorObjects(
            Class<T> clazz,
            Date dateLimit);

}
